#include <core/ecs/filters/filterregistry.hpp>
#include <core/ecs/registry.hpp>

namespace legion::core::ecs
{
    void FilterRegistry::markComponentAdd(id_type componentId, entity target)
    {
        auto& composition = Registry::entityComposition(target);

        for (auto& filter : filters())
            if (filter->contains(componentId) && filter->contains(composition))
                entityLists().at(filter->id()).insert(target);

    }

    void FilterRegistry::markComponentErase(id_type componentId, entity target)
    {
        for (auto& filter : filters())
            if (filter->contains(componentId))
                entityLists().at(filter->id()).erase(target); // Will not do anything if the target wasn't in the set.

    }

    void FilterRegistry::markEntityDestruction(entity target)
    {
        for (auto& [_, entityList] : entityLists())
            entityList.erase(target); // Will not do anything if the target wasn't in the set.
    }

    void FilterRegistry::markEntityFullCreation(entity target)
    {
        auto& composition = Registry::entityComposition(target);

        for (auto& filter : filters())
            if (filter->contains(composition))
                entityLists().at(filter->id()).insert(target);
    }

    entity_set& FilterRegistry::getList(id_type filterId)
    {
        return entityLists().at(filterId);
    }

    L_NODISCARD std::unordered_map<id_type, entity_set>& FilterRegistry::entityLists() noexcept
    {
        static std::unordered_map<id_type, entity_set> m_entityLists;
        return m_entityLists;
    }

    L_NODISCARD std::vector<std::unique_ptr<filter_info_base>>& FilterRegistry::filters() noexcept
    {
        static std::vector<std::unique_ptr<filter_info_base>> m_filters;
        return m_filters;
    }

}
