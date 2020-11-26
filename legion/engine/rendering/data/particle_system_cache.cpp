#include <rendering/data/particle_system_cache.hpp>
#include <rendering/data/particle_system_base.hpp>

namespace legion::rendering
{
    std::unordered_map<id_type, std::unique_ptr<const ParticleSystemBase>> ParticleSystemCache::m_cache;
    async::readonly_rw_spinlock ParticleSystemCache::m_particleSystemLock;

    ParticleSystemHandle ParticleSystemCache::createParticleSystemImpl(std::string name, ParticleSystemBase* ptr)
    {
        std::unique_ptr<ParticleSystemBase> particleSystem(ptr);
        id_type id = nameHash(name);
        async::readwrite_guard guard(m_particleSystemLock);
        auto iterator = m_cache.emplace(id, std::move(particleSystem));
        if(iterator.second)
            return ParticleSystemHandle{ iterator.first->first };
        return ParticleSystemHandle{ invalid_id };
    }

}
