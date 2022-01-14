#include <core/ecs/filters/filterregistry.hpp>
#include <core/ecs/registry.hpp>

namespace legion::core::ecs
{
    void FilterRegistry::markComponentAdd(id_type componentId, entity target)
    {
        // Get the entities current component composition.
        auto& composition = Registry::entityComposition(target);
        auto& lists = entityLists();

        for (auto& filter : filters()) // Walk all filters and check if they care about the current component type.
            if (filter->contains(componentId) && filter->contains(composition)) // If they do, then check if the current entity falls into that filter.
                lists.at(filter->id()).insert(target); // Insert entity in the entity list of the filter if the entity fits the requirements.

    }

    void FilterRegistry::markComponentErase(id_type componentId, entity target)
    {
        auto& lists = entityLists();

        for (auto& filter : filters()) // Walk all filters and check if they care about the current component type.
            if (filter->contains(componentId)) // If they do, then erase the entity from their list if it is in their list.
                lists.at(filter->id()).erase(target); // Will not do anything if the target wasn't in the set.

    }

    void FilterRegistry::markEntityDestruction(entity target)
    {
        for (auto& [_, entityList] : entityLists()) //Walk all entity lists and erase this entity from them is it's in the list.
            entityList.erase(target); // Will not do anything if the target wasn't in the set.
    }

    void FilterRegistry::markEntityFullCreation(entity target)
    {
        // Get the entities current component composition.
        auto& composition = Registry::entityComposition(target);

        auto& lists = entityLists();

        for (auto& filter : filters()) // Walk all filters and check if the new entity satisfies the requirements.
            if (filter->contains(composition))
                lists.at(filter->id()).insert(target); // Insert entity in the entity list of the filter if the entity fits the requirements.
    }

    entity_set& FilterRegistry::getList(id_type filterId)
    {
        return entityLists().at(filterId);
    }

    void FilterRegistry::onInit()
    {
        reportDependency<::legion::core::detail::type_data>();
        reportDependency<Registry>();
        create();
    }

    void FilterRegistry::onShutdown()
    {
        for (auto& [_, list] : entityLists())
            list.clear();
    }

    std::unordered_map<id_type, entity_set>& FilterRegistry::entityLists()
    {
        static std::unordered_map<id_type, entity_set> m_entityLists;
        return m_entityLists;
    }

    std::vector<std::unique_ptr<filter_info_base>>& FilterRegistry::filters()
    {
        static std::vector<std::unique_ptr<filter_info_base>> m_filters;
        return m_filters;
    }

}
