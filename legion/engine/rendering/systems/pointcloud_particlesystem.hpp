#pragma once
#include <rendering/data/particle_system_base.hpp>
#include <rendering/debugrendering.hpp>
#include <core/core.hpp>
#include <rendering/data/Octree.hpp>
#include <rendering/components/lod.hpp>
#include <random>
#include<rendering/components/point_emitter_data.hpp>
#include <rendering/util/bindings.hpp>
#include <application/application.hpp>
#include<core/core.hpp>

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
        //Create data component
        auto emitterDataHandle = emitter_handle.entity.add_component<rendering::point_emitter_data>();
        auto emitterData = emitterDataHandle.read();

        //define octree bounds
        float minX = std::numeric_limits<float>().max();
        float maxX = std::numeric_limits<float>().min();

        float minY = std::numeric_limits<float>().max();
        float maxY = std::numeric_limits<float>().min();

        float minZ = std::numeric_limits<float>().max();
        float maxZ = std::numeric_limits<float>().min();
        //shuffle input
        /*auto rng = std::default_random_engine{};
        std::shuffle(m_positions.begin(), m_positions.end(), rng);*/
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
        //Create Octree
        emitterData.Tree = new rendering::Octree<uint8>(8, min, max);
        //insert points into tree
        for (auto position : m_positions)
        {
            emitterData.Tree->insertNode(0, position);
        }
        //Write to handle
        emitterDataHandle.write(emitterData);
        //create the particles
        populateEmitter(emitter_handle, emitterDataHandle);
    }
    //iterates input posiitons and creates particles based on position
    /**
     * @brief Creates particles based on position for the emitter, checks for recycling
     */
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
    /**
     * @brief Decreases the particles detail down to the specified target LOD
     */
    void decreaseDetail(rendering::particle_emitter& emitter, rendering::point_emitter_data& data, int targetLod, int maxLod) const
    {
        //   log::debug("decreasing detail");

           //read emitter
        if (emitter.livingParticles.size() == 0) return;
        //get the amount of particles to remove
    //    log::debug("particles: " + std::to_string(emitter.livingParticles.size()));
        int targetParticleCount = data.ElementsPerLOD.at(maxLod - targetLod);
        ///   log::debug("target count: " + std::to_string(targetParticleCount));

        int delta = emitter.livingParticles.size() - targetParticleCount;
        //remove particles from the end of the living particles
        //living particles should be stored in order of detail
        for (size_t i = 1; i < delta + 1; i++)
        {
            cleanUpParticle(emitter.livingParticles.at(emitter.livingParticles.size() - 1), emitter);
        }
        data.CurrentLOD = targetLod;
    }
    /**
    * @brief Increases the particles up to the specified target LOD
    */
    void increaseDetail(rendering::particle_emitter& emitter, rendering::point_emitter_data& data, int targetLod, int maxLod, rendering::lod& lod) const
    {
        if (!data.Tree) return;

        //create data container
        std::vector<math::vec3>* newData = new std::vector<math::vec3>();
        //populate emitter progressively for each LOD
        data.Tree->GetDataRange(lod.MaxLod - data.CurrentLOD + 1, lod.MaxLod - targetLod + 1, newData);

        CreateParticles(newData, emitter);
        newData->clear();
        data.CurrentLOD = targetLod;
    }
    /**
     * @brief populates the particle emitter with particles, creates LOD component and an Octree
     */
    void populateEmitter(ecs::component_handle<rendering::particle_emitter> emitter_handle, ecs::component_handle<rendering::point_emitter_data> data) const
    {
        //read particle emitter if tree is null something went wrong, return
        rendering::particle_emitter emitter = emitter_handle.read();
        auto emitterData = data.read();
        if (!emitterData.Tree) return;
        int maxTreeDepth = emitterData.Tree->GetTreeDepth();


        //create data container
        std::vector<math::vec3>* newData = new std::vector<math::vec3>();
        //populate emitter progressively for each LOD
        int particleCount = 0;
        int LODcount = 0;
        for (size_t i = 0; i < maxTreeDepth; i++)
        {
            emitterData.Tree->GetDataRange(i, (i + 1), newData);
            particleCount += newData->size();
            //exit loop if there is no new data to be found
            if (newData->size() == 0) break;
            CreateParticles(newData, emitter);
            newData->clear();
            LODcount++;
            //store the amount of particles for the lod so that we can later easily remove them again
            emitterData.ElementsPerLOD.push_back(particleCount);

        }
        rendering::lod lodComponent = rendering::lod(LODcount);
        emitter_handle.entity.add_component<rendering::lod>(lodComponent);
        emitterData.CurrentLOD = 0;
        data.write(emitterData);
        emitter_handle.write(emitter);
    }
    /**
     * @brief Checks if there has been LOD changes, decreases or increases LOD
     */
    void update(std::vector<ecs::entity_handle>& entities, ecs::component_handle<rendering::particle_emitter> emitterHandle, time::span) const override
    {
        //check if buffer has been read and has not been overwritten yet 
        if (!m_overwrittenColorBuffer && m_particleModel.is_buffered())
        {
            //Get a window and lock
            auto window = ecs::EcsRegistry::world.read_component<app::window>();
            app::context_guard guard(window);
            if (guard.contextIsValid())
            {
                //for now just create a random buffer
                std::random_device rd;

                std::mt19937_64 rng(rd());
                std::uniform_real_distribution<double> unif(0, 1);

                std::vector<math::color> randColors;
                for (size_t i = 0; i < 150000; i++)
                {
                    double rand1 = unif(rng);
                    double rand2 = unif(rng);
                    double rand3 = unif(rng);
                    math::color newColor = math::color(rand1, rand2, rand3, 1);
                    randColors.push_back(newColor);
                }
                //create buffer
                rendering::buffer colorBuffer = rendering::buffer(GL_ARRAY_BUFFER, randColors, GL_STATIC_DRAW);
                auto& model = m_particleModel.get_model();
                m_particleModel.overwrite_buffer(colorBuffer, SV_COLOR, true);
                m_overwrittenColorBuffer = true;
            }
        }

        auto lodComponent = emitterHandle.entity.get_component_handle<rendering::lod>().read();
        rendering::particle_emitter emitter = emitterHandle.read();
        auto emitterDataHandle = emitterHandle.entity.get_component_handle<rendering::point_emitter_data>();
        auto emitterData = emitterDataHandle.read();

        if (emitterData.CurrentLOD != lodComponent.Level)
        {
            if (emitterData.CurrentLOD > lodComponent.Level)
            {
                increaseDetail(emitter, emitterData, lodComponent.Level, lodComponent.MaxLod, lodComponent);
            }
            else
            {
                decreaseDetail(emitter, emitterData, lodComponent.Level, lodComponent.MaxLod);
            }
            emitterData.CurrentLOD = lodComponent.Level;
            emitterHandle.write(emitter);
            emitterDataHandle.write(emitterData);
        }
    }

private:
    std::vector<math::vec3> m_positions;
    mutable bool m_overwrittenColorBuffer = false;
};
