#pragma once
#include <rendering/data/particle_system_base.hpp>
#include <rendering/debugrendering.hpp>
#include <core/core.hpp>
#include <rendering/data/Octree.hpp>
#include <rendering/components/lod.hpp>
using namespace legion;
/**
 * @struct pointCloudParameters
 * @brief A struct that simplifies the parameter input of the particle system constructor.
 */
struct pointCloudParameters
{
    math::vec3 startingSize;
    rendering::material_handle particleMaterial;
    rendering::model_handle particleModel;

    bool looping = false;
    float maxLifeTime = 0.0f;
    float startingLifeTime = 0.0f;
    uint spawnRate = 0;
    uint particleCount = 0;
    uint maxParticles = 0;
    math::vec3 startingVel = math::vec3(0);
    float sizeOverLifeTime = 0.0f;
};

class PointCloudParticleSystem : public rendering::ParticleSystemBase
{
public:
    /**
     * @brief Constructor of the point cloud particle system.
     * @param params A struct with a bunch of default parameters and some parameters needed to be set.
     * @param positions A list of positions that the particle system uses to create its particles at.
     */
    PointCloudParticleSystem(pointCloudParameters params, const std::vector<math::vec3>& positions)
    {
        m_looping = params.looping;
        m_maxLifeTime = params.maxLifeTime;
        m_startingLifeTime = params.startingLifeTime;
        m_spawnRate = params.spawnRate;
        m_particleCount = params.particleCount;
        m_maxParticles = params.maxParticles;
        m_startingVelocity = params.startingVel;
        m_startingSize = params.startingSize;
        m_sizeOverLifetime = params.sizeOverLifeTime;
        m_particleMaterial = params.particleMaterial;
        m_particleModel = params.particleModel;
        m_positions = positions;
    }
    /**
     * @brief Setup function that will be called to populate the emitter with the required particles.
     * @param emitter_handle The emitter that you are populating.
     */
    void setup(ecs::component_handle<rendering::particle_emitter> emitter_handle) const override
    {
        //Reads emitter
        rendering::particle_emitter emitter = emitter_handle.read();

        float minX = std::numeric_limits<float>().max();
        float maxX = std::numeric_limits<float>().min();

        float minY = std::numeric_limits<float>().max();
        float maxY = std::numeric_limits<float>().min();

        float minZ = std::numeric_limits<float>().max();
        float maxZ = std::numeric_limits<float>().min();

        for (auto position : m_positions)
        {
            if (position.x < minX) minX = position.x;
            if (position.x > maxX) maxX = position.x;
            if (position.y < minY) minY = position.y;
            if (position.y > maxY) maxY = position.y;
            if (position.z < minZ) minZ = position.z;
            if (position.z > maxZ) maxZ = position.z;
        }
        float universalMin = math::min(minX, math::min(minY, minX));
        float universalMax = math::max(maxX, math::max(maxY, maxX));

        math::vec3 min = math::vec3(universalMin);
        math::vec3 max = math::vec3(universalMax);
        emitter.Tree = new rendering::Octree<uint8>(8, min, max);

        //  emitter.Tree.GenerateAverage();
        log::debug("got points: ");
        log::debug(m_positions.size());

        for (auto position : m_positions)
        {
            emitter.Tree->insertNode(0, position);
        }
        // m_maxLevel = emitter.Tree->GetTreeDepth();
      //   log::debug(m_maxLevel);
         //make sure to write so that populate emitter gets the generated tree data
        emitter_handle.write(emitter);
        populateEmitter(emitter_handle);
    }
    //iterates input posiitons and creates particles based on position
    void CreateParticles(std::vector<math::vec3>* inputData, rendering::particle_emitter& emitter) const
    {
        for (auto item : *inputData)
        {
            ecs::component_handle<rendering::particle> particleComponent = checkToRecycle(emitter);
            auto ent = particleComponent.entity;

            //Gets position, rotation and scale of entity.
            auto trans = ent.get_component_handles<transform>();
            auto& [pos, _, scale] = trans;

            //Sets the particle scale to the right scale.
            pos.write(item);
            scale.write(math::vec3(m_startingSize));

            //Populates the particle with the appropriate stuffs.
            createParticle(particleComponent, trans);
        }
    }
    void decreaseDetail(rendering::particle_emitter& emitter, int targetLod, int maxLod) const
    {
        log::debug("decreasing detail");

        //read emitter
        if (emitter.livingParticles.size() == 0) return;
        //get the amount of particles to remove
        int targetParticleCount = emitter.ElementsPerLOD.at(maxLod - targetLod);
        int delta = emitter.livingParticles.size() - targetParticleCount;
        //remove particles from the end of the living particles
        //living particles should be stored in order of detail
        for (size_t i = 1; i < delta + 1; i++)
        {
            cleanUpParticle(emitter.livingParticles.at(emitter.livingParticles.size() - 1), emitter);
        }
    }
    void increaseDetail(rendering::particle_emitter& emitter, int targetLod, int maxLod, rendering::lod& lod) const
    {
        log::debug("increasing detail");
        if (!emitter.Tree) return;
        //get lod component 
        int maxLOD = lod.MaxLod;
        int maxTreeDepth = emitter.Tree->GetTreeDepth();
        //calculates how many tree depth levels are on LOD 
        int treeStep = maxTreeDepth / maxLOD;
        //create data container
        std::vector<math::vec3>* newData = new std::vector<math::vec3>();
        //populate emitter progressively for each LOD
        int particleCount = 0;
        for (size_t i = targetLod; i < emitter.CurrentLOD; i++)
        {
            emitter.Tree->GetDataRange(i, (i + 1), newData);
            particleCount += newData->size();
            emitter.ElementsPerLOD.push_back(particleCount);
            CreateParticles(newData, emitter);
            newData->clear();
        }
    }
    void populateEmitter(ecs::component_handle<rendering::particle_emitter> emitter_handle) const
    {
        OPTICK_EVENT();
        //read particle emitter if tree is null something went wrong, return
        rendering::particle_emitter emitter = emitter_handle.read();
        if (!emitter.Tree) return;
        //get lod component 
        auto lod = emitter_handle.entity.get_component_handle<rendering::lod>().read();
        int maxLOD = lod.MaxLod;
        int maxTreeDepth = emitter.Tree->GetTreeDepth();
        //calculates how many tree depth levels are on LOD 
        int treeStep = maxTreeDepth / maxLOD;
        //create data container
        std::vector<math::vec3>* newData = new std::vector<math::vec3>();
        //populate emitter progressively for each LOD
        int particleCount = 0;
        for (size_t i = 0; i < maxLOD; i++)
        {
            log::debug(std::to_string(newData->size()));
            emitter.Tree->GetDataRange(i, (i + 1), newData);
            particleCount += newData->size();
            emitter.ElementsPerLOD.push_back(particleCount);
            CreateParticles(newData, emitter);
            newData->clear();
        }

        emitter_handle.write(emitter);
    }


    void update(std::vector<ecs::entity_handle>& entities, ecs::component_handle<rendering::particle_emitter> emitterHandle, time::span) const override
    {
        OPTICK_EVENT();
        auto lodComponent = emitterHandle.entity.get_component_handle<rendering::lod>().read();
        rendering::particle_emitter emitter = emitterHandle.read();

        if (emitter.CurrentLOD != lodComponent.Level)
        {
            if (emitter.CurrentLOD > lodComponent.Level)
            {
                increaseDetail(emitter, lodComponent.Level, lodComponent.MaxLod, lodComponent);
            }
            else
            {
                decreaseDetail(emitter, lodComponent.Level, lodComponent.MaxLod);
            }
            emitter.CurrentLOD = lodComponent.Level;
            emitterHandle.write(emitter);

        }
    }

private:
    std::vector<math::vec3> m_positions;
};
