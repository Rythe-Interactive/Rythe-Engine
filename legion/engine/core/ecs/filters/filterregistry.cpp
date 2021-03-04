#include <core/ecs/filters/filterregistry.hpp>

namespace legion::core::ecs
{
    std::unordered_map<id_type, hashed_sparse_set<entity>> FilterRegistry::m_entityLists;
    std::vector<std::unique_ptr<filter_info_base>> FilterRegistry::m_filters;

    void FilterRegistry::markEntityDestruction(entity target)
    {
        for (auto& [_, entityList] : m_entityLists)
            entityList.erase(target); // Will not do anything if the target wasn't in the set.
    }

}
