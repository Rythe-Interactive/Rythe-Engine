#pragma once
#include <core/core.hpp>
#include <rendering/components/particle.hpp>
#include <unordered_map>
#include <type_traits>


namespace legion::rendering
{
    struct particle_emitter;

    class ParticleSystemBase
    {
    public:
        virtual void setup(ecs::component_handle<particle_emitter>) LEGION_IMPURE;

        virtual void update(std::vector<ecs::entity_handle>, ecs::component_handle<particle_emitter>) LEGION_IMPURE;
    private:
        virtual ecs::component_handle<particle> createParticle(
            ecs::component_handle<transform> transformHandle,
            ecs::component_handle<particle_emitter> emitterHandle);

        void cleanUpParticle(
            ecs::component_handle<particle> particleHandle,
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
    };

    struct ParticleSystemHandle
    {
        id_type id;
        const ParticleSystemBase* get() const;
        bool validate() const noexcept;
    };

    class ParticleSystemCache
    {
        friend struct ParticleSystemHandle;
    public:
        template<class T, class ... Args>
        static ParticleSystemHandle createParticleSystem(std::string name, Args&&...args)
        {
            static_assert(std::is_base_of<ParticleSystemBase, T>::value,"T must inherit from ParticleSystemBase!");
            std::unique_ptr<T> particleSystem(new T(args...));
            id_type id = nameHash(name);
            async::readwrite_guard guard(m_particleSystemLock);
            m_cache.insert({ id, particleSystem });
        }
        static ParticleSystemHandle getParticleSystem(std::string name)
        {
            const auto id = nameHash(name);
            async::readonly_guard guard(m_particleSystemLock);
            if (m_cache.find(id) == m_cache.end()) return ParticleSystemHandle{ invalid_id };
            return ParticleSystemHandle{ id };
        }
    private:
        static const ParticleSystemBase* getParticleSystemPointer(id_type id)
        {
            const auto iterator = m_cache.find(id);
            if (iterator == m_cache.end()) return nullptr;
            return iterator->second.get();
        }

        static std::unordered_map<id_type, std::unique_ptr<const ParticleSystemBase>> m_cache;
        static async::readonly_rw_spinlock m_particleSystemLock;
    };

    inline const ParticleSystemBase* ParticleSystemHandle::get() const
    {
        return ParticleSystemCache::getParticleSystemPointer(id);
    }

    inline bool ParticleSystemHandle::validate() const noexcept
    {
        if (ParticleSystemCache::getParticleSystemPointer(id) == nullptr) return false;
        return true;
    }
}
