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
            emitters.queryEntities();
            for (auto entity : emitters)
            {
                //Gets emitter handle and emitter.
                auto emitterHandle = entity.get_component_handle<particle_emitter>();
                auto emit = emitterHandle.read();
                //Checks if emitter was already initialized.
                if(!emit.setupCompleted)
                {
                    //If NOT then it goes through the particle system setup.
                    emit.setupCompleted = true;
                    emitterHandle.write(emit);

                    const ParticleSystemBase* particleSystem = emit.particleSystemHandle.get();
                    particleSystem->setup(emitterHandle);
                }
                else
                {
                    //If it IS then it runs the emitter through the particle system update.
                    std::vector<ecs::entity_handle> particles = emit.livingParticles;
                    const ParticleSystemBase* particleSystem = emit.particleSystemHandle.get();
                    particleSystem->update(particles, emitterHandle,deltaTime);
                }
            }
        }
    };
}
