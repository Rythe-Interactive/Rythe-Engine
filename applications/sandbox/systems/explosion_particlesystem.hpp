#pragma once
#include <rendering/data/particle_system_base.hpp>

#include <rendering/systems/renderer.hpp>

using namespace legion;
/**
 * @struct pointCloudParameters
 * @brief A struct that simplifies the parameter input of the particle system constructor.
 */
struct explosionParameters
{
    math::vec3 startingSize;
    rendering::material_handle particleMaterial;
    rendering::model_handle particleModel;
    float maxLifeTime;
    math::vec3 startingVel;
    float decelerationScalar;
    float sizeOverLifeTime;
    math::color endColor;

    float startingLifeTime = 0.0f;
    bool looping = false;
    uint spawnRate = 0;
    uint particleCount = 0;
    uint maxParticles = 0;
};

class ExplosionParticleSystem : public rendering::ParticleSystemBase
{
public:

    ExplosionParticleSystem(explosionParameters params)
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

        m_decelerationScalar = params.decelerationScalar;
        m_endColor = params.endColor;
        m_beginColor = m_particleMaterial.get_param<math::vec4>("color");
    }

    void setup(ecs::component_handle<rendering::particle_emitter> emitter_handle) const override
    {
        auto vertPositions = m_particleModel.get_mesh().get().second.vertices;
        //emitter positions/origin
        const auto emitterPosHandle = emitter_handle.entity.get_component_handle<position>();
        const math::vec3 emitterPos = emitterPosHandle.read();

        auto emitter = emitter_handle.read();
        //std::vector<math::vec3> directions = emitter.directions;
        time::clock<> timer;
        for (math::vec3 vert_position : vertPositions)
        {
            timer.start();
#pragma region Create particle
                //Checks the emitter if it has a recycled particle to use, if not it creates a new one.
            ecs::component_handle<rendering::particle> particleComponent = checkToRecycle(emitter_handle);
            auto ent = particleComponent.entity;
            //Checks if the entity has a transform, if not it adds one.
            if (!ent.has_components<transform>())
                ent.add_components<transform>();

            //Gets position, rotation and scale of entity.
            auto trans = ent.get_component_handles<transform>();
            auto& [pos, _, scale] = trans;

            //Sets the particle scale to the right scale.
            auto newPos = emitterPos + vert_position;
            pos.write(newPos);
            scale.write(math::vec3(m_startingSize));

            //Populates the particle with the appropriate stuffs.
            createParticle(particleComponent, trans);
            log::debug("create particle took: {} ms", timer.restart().milliseconds());
#pragma endregion 
#pragma region Set directions
            math::vec3 pointPos = newPos;
            math::vec3 pointDirection = math::normalize(pointPos - emitterPos);
            //if (emitter.particleIsAlive(particleComponent))
            //    directions.emplace(directions.end(),pointDirection);
            log::debug("set directions took: {} ms", timer.restart().milliseconds());
#pragma endregion
#pragma region Set parameter values
            rendering::particle particularParticle = particleComponent.read();
            particularParticle.lifeTime = 0;
            particularParticle.particleVelocity = m_startingVelocity.x * pointDirection;
            particleComponent.write(particularParticle);
#pragma endregion
            log::debug("set parameters took: {} ms", timer.restart().milliseconds());
        }
    }

    void update(std::vector<ecs::entity_handle> particle_list, ecs::component_handle<rendering::particle_emitter> particle_emitter, time::span delta_time) const override
    {
        auto emitter = particle_emitter.read();
        for (int i = 0; i < particle_list.size(); i++)
        {
            auto particleEnt = particle_list[i];

            auto particleHandle = particleEnt.get_component_handle<rendering::particle>();
            rendering::particle particle = particleHandle.read();

            if (particle.lifeTime == -9)
            {
#pragma region Check if still alive

                //const int index = FindIndexOf(particle_list, particleEnt);
                cleanUpParticle(particleHandle, particle_emitter);
                //auto toRemove = std::remove(emitter.directions.begin(), emitter.directions.end(), emitter.directions[index]);
                //emitter.directions.erase(toRemove);
#pragma endregion 
            }
            else
            {
                //Get transform
                auto [position, _, scale] = particleEnt.get_component_handles<transform>();
                //Update position
                math::vec3 pos = position.read();
                pos.x += particle.particleVelocity.x * delta_time;
                pos.y += particle.particleVelocity.y * delta_time;
                pos.z += particle.particleVelocity.z * delta_time;
                position.write(pos);
                //Update size
                math::vec3 size = scale.read();
                size *= m_sizeOverLifetime;
                scale.write(size);
                //Update velocity
                particle.particleVelocity *= m_decelerationScalar;

                //check if first particle, because material is universal. dont wanna set it every frame
                if (i == 0)
                {
                    //Update color
                    auto meshRenderer = particleEnt.get_component_handle<rendering::mesh_renderer>().read();
                    rendering::material_handle matHandle = meshRenderer.material;
                    matHandle.set_param("color", LinearInterpolateColor(m_beginColor, m_endColor, particle.lifeTime, m_maxLifeTime));
                }

                //update lifetime
                particle.lifeTime += delta_time;

                particleHandle.write(particle);
            }
        }
    }

private:

    static const int FindIndexOf(std::vector<ecs::entity_handle> list, ecs::entity_handle entity)
    {
        for (int i = 0; i < list.size(); i++)
        {
            if (list[i] == entity) return i;
        }
        return -1;
    }
    static const math::color LinearInterpolateColor(math::color beginColor, math::color endColor, float currentLifetime, float maxLifetime)
    {
        float r = beginColor.r + (endColor.r - beginColor.r) / maxLifetime * currentLifetime;
        float g = beginColor.g + (endColor.g - beginColor.g) / maxLifetime * currentLifetime;
        float b = beginColor.b + (endColor.b - beginColor.b) / maxLifetime * currentLifetime;
        float a = beginColor.a + (endColor.a - beginColor.a) / maxLifetime * currentLifetime;

        return math::color(r, b, g, a);
    }

    float m_decelerationScalar;
    math::color m_beginColor;
    math::color m_endColor;
};
