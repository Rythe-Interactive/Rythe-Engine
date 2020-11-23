#pragma once
#include <core/core.hpp>
#include <rendering/components/particle.hpp>
#include <unordered_map>
#include <rendering/components/particle_emitter.hpp>

namespace legion::rendering
{
    struct ParticleSystemHandle
    {
        id_type id;
    };

    class ParticleSystemBase
    {
    public:
        ParticleSystemBase();
        ~ParticleSystemBase();
        void setup(ecs::component_handle<particle_emitter>);
        void update(std::vector<ecs::entity_handle>, ecs::component_handle<particle_emitter>);
    private:
        void createParticle(ecs::component_handle<transform>, ecs::component_handle<particle_emitter>);
        void cleanUpParticle(ecs::component_handle<particle> , ecs::component_handle<particle_emitter>);

        bool m_looping;

        float m_maxLifeTime;
        float m_startingLifeTime;

        uint m_spawnRate;
        uint m_particleCount;
        uint m_maxParticles;

        math::vec3 m_startingVelocity;

        math::vec3 m_startingSize;
        float m_sizeOverLifetime;
    };

    class ParticleSystemCache
    {
    public:
        ParticleSystemCache() = default;
        ~ParticleSystemCache() =  default;
        ParticleSystemHandle createParticleSystem(std::string);
        ParticleSystemHandle getParticleSystem(std::string);
    private:
        std::unordered_map<id_type, ParticleSystemBase*> m_cache;
        static async::readonly_rw_spinlock m_particleSystemLock;
    };
}
