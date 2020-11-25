#pragma once
#include <core/core.hpp>
#include <rendering/data/particle_system_base.hpp>

namespace legion::rendering
{
    class ParticleSystemManager : public System<ParticleSystemManager>
    {

        ParticleSystemManager()
        {
            ParticleSystemBase::m_registry = m_ecs;
        }

        void setup()
        {
            createProcess<&ParticleSystemManager::update>("Update");

            auto emitters = createQuery<particle_emitter>();
            for (auto entity : emitters)
            {
                ecs::component_handle<particle_emitter> emitterHandle = entity.get_component_handle<particle_emitter>();
                ParticleSystemBase particleSystem = *emitterHandle.read().particleSystemHandle.get();
                particleSystem.setup(emitterHandle);
            }
        }

        void update(time::span deltaTime)
        {
            auto emitters = createQuery<particle_emitter>();
            for (auto entity : emitters)
            {
                ecs::component_handle<particle_emitter> emitterHandle = entity.get_component_handle<particle_emitter>();
                particle_emitter emitter = emitterHandle.read();
                std::vector<ecs::entity_handle> particles = emitter.livingParticles;
                ParticleSystemBase particleSystem = *emitter.particleSystemHandle.get();
                particleSystem.update(particles, emitterHandle);
            }
        }
    };
}
