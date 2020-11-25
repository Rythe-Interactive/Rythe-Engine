#pragma once
#include <core/core.hpp>
#include <rendering/components/particle.hpp>
#include <rendering/components/particle_emitter.hpp>
#include <rendering/data/material.hpp>

namespace legion::rendering
{
    class ParticleSystemBase
    {
        friend class ParticleSystemManager;
    public:
        virtual void setup(ecs::component_handle<particle_emitter>) LEGION_IMPURE;

        virtual void update(std::vector<ecs::entity_handle>, ecs::component_handle<particle_emitter>) LEGION_IMPURE;

    private:
        virtual void createParticle(
            ecs::component_handle<particle> particleHandle,
            ecs::component_handle<transform> transformHandle,
            ecs::component_handle<particle_emitter> emitterHandle);

        void cleanUpParticle(
            ecs::component_handle<particle> particleHandle,
            ecs::component_handle<particle_emitter> emitterHandle);

        ecs::component_handle<particle> checkToRecycle(
            ecs::component_handle<particle_emitter> emitterHandle);

        bool m_looping;

        float m_maxLifeTime;
        float m_startingLifeTime;

        uint m_spawnRate;
        uint m_particleCount;
        uint m_maxParticles;

        math::vec3 m_startingVelocity;

        math::vec3 m_startingSize;
        float m_sizeOverLifetime;

        material_handle m_particleMaterial;
        model_handle m_particleModel;

        static ecs::EcsRegistry* m_registry;
    };
}
