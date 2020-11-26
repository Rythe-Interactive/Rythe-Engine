#pragma once
#include <core/core.hpp>
#include <rendering/data/particle_system_base.hpp>

namespace legion::rendering
{
    /**
     * @class ParticleSystemManager
     * @brief The class used to update all particles in every emitter.
     */
    class ParticleSystemManager : public System<ParticleSystemManager>
    {
    public:
        ParticleSystemManager()
        {
            ParticleSystemBase::m_registry = m_ecs;
        }
        /**
         * @brief Sets up the particle system manager.
         */
        void setup()
        {
            createProcess<&ParticleSystemManager::update>("Update");
        }
        /**
         * @brief Every frame, goes through every emitter and updates their particles with their respective particle systems.
         * @param deltaTime The delta time to be used inside of the update.
         */
        void update(time::span deltaTime)
        {
            static auto emitters = createQuery<particle_emitter>();
            for (auto entity : emitters)
            {
                if(!entity.get_component_handle<particle_emitter>().read().setupCompleted)
                {
                    auto particleEmitHandl = entity.get_component_handle<particle_emitter>().read();
                    particleEmitHandl.setupCompleted = true;
                    entity.get_component_handle<particle_emitter>().write(particleEmitHandl);

                    ecs::component_handle<particle_emitter> emitterHandle = entity.get_component_handle<particle_emitter>();
                    const ParticleSystemBase* particleSystem = emitterHandle.read().particleSystemHandle.get();
                    particleSystem->setup(emitterHandle);
                }
                else
                {
                    ecs::component_handle<particle_emitter> emitterHandle = entity.get_component_handle<particle_emitter>();
                    particle_emitter emitter = emitterHandle.read();
                    std::vector<ecs::entity_handle> particles = emitter.livingParticles;
                    const ParticleSystemBase* particleSystem = emitter.particleSystemHandle.get();
                    particleSystem->update(particles, emitterHandle);
                }
            }
        }


    };
}
