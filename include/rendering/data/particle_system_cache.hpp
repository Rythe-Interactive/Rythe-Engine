#pragma once
#include <core/core.hpp>

namespace legion::rendering
{
    class ParticleSystemBase;

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
            static_assert(std::is_base_of<ParticleSystemBase, T>::value, "T must inherit from ParticleSystemBase!");
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
