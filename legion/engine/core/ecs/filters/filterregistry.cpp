#include <core/ecs/filters/filterregistry.hpp>
#include <core/ecs/registry.hpp>

namespace legion::core::ecs
{
    std::unordered_map<id_type, hashed_sparse_set<entity>> FilterRegistry::m_entityLists;
    std::vector<std::unique_ptr<filter_info_base>> FilterRegistry::m_filters;

    void FilterRegistry::markComponentAdd(id_type componentId, entity target)
    {
        auto& composition = Registry::entityComposition(target);

        for (auto& filter : m_filters)
            if (filter->contains(componentId) && filter->contains(composition))
                m_entityLists.at(filter->id()).insert(target);

    }

    void FilterRegistry::markComponentErase(id_type componentId, entity target)
    {
        for (auto& filter : m_filters)
            if (filter->contains(componentId))
                m_entityLists.at(filter->id()).erase(target); // Will not do anything if the target wasn't in the set.

    }

    void FilterRegistry::markEntityDestruction(entity target)
    {
        for (auto& [_, entityList] : m_entityLists)
            entityList.erase(target); // Will not do anything if the target wasn't in the set.
    }

    void FilterRegistry::markEntityFullCreation(entity target)
    {
        auto& composition = Registry::entityComposition(target);

        for (auto& filter : m_filters)
            if (filter->contains(composition))
                m_entityLists.at(filter->id()).insert(target);
    }

    entity_set& FilterRegistry::getList(id_type filterId)
    {
        return m_entityLists.at(filterId);
    }

}
