#pragma once
#include <core/core.hpp>
#include <rendering/components/particle.hpp>
#include <unordered_map>
#include <rendering/components/particle_emitter.hpp>

namespace legion::rendering
{
    

    class ParticleSystemBase
    {
    public:
        virtual void setup(ecs::component_handle<particle_emitter>) LEGION_IMPURE;

        virtual void update(std::vector<ecs::entity_handle>, ecs::component_handle<particle_emitter>)
        {
            
        }
    private:
        virtual void createParticle(ecs::component_handle<transform>, ecs::component_handle<particle_emitter>)
        {
            
        }
        virtual void cleanUpParticle(ecs::component_handle<particle> , ecs::component_handle<particle_emitter>)
        {
            
        }

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

    struct ParticleSystemHandle
    {
        id_type id;
        const ParticleSystemBase* get();
    };

    class ParticleSystemCache
    {
        friend struct ParticleSystemHandle;
    public:
        static ParticleSystemHandle createParticleSystem(std::string);
        static ParticleSystemHandle getParticleSystem(std::string);
    private:
        static const ParticleSystemBase* getParticleSystemPointer(id_type);
        static std::unordered_map<id_type, std::unique_ptr<const ParticleSystemBase>> m_cache;
        static async::readonly_rw_spinlock m_particleSystemLock;
    };

    const ParticleSystemBase* ParticleSystemHandle::get()
    {
        return ParticleSystemCache::getParticleSystemPointer(id);
    }
}
