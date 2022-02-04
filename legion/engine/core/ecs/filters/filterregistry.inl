#include <core/ecs/registry.hpp>
#pragma once

namespace legion::core::ecs
{
    template<typename component_type>
    inline void FilterRegistry::markComponentAdd(entity target)
    {
        // Get the entities current component composition.
        auto& composition = Registry::entityComposition(target);
        auto& lists = entityLists();

        for (auto& filter : filters()) // Walk all filters and check if they care about the current component type.
            if (filter->contains(make_hash<component_type>()) && filter->contains(composition)) // If they do, then check if the current entity falls into that filter.
                lists.at(filter->id()).insert(target); // Insert entity in the entity list of the filter if the entity fits the requirements.
    }

    template<typename component_type>
    inline void FilterRegistry::markComponentErase(entity target)
    {
        auto& lists = entityLists();

        for (auto& filter : filters()) // Walk all filters and check if they care about the current component type.
            if (filter->contains(make_hash<component_type>())) // If they do, then erase the entity from their list if it is in their list.
                lists.at(filter->id()).erase(target); // Will not do anything if the target wasn't in the set.
    }

    template<typename component_type>
    inline constexpr id_type FilterRegistry::generateId() noexcept
    {
        return make_hash<component_type>();
    }

    template<typename component_type0, typename component_type1, typename... component_types>
    inline constexpr id_type FilterRegistry::generateId() noexcept
    {
        return combine_hash(make_hash<component_type0>(), generateId<component_type1, component_types...>());
    }

    template<typename... component_types>
    inline id_type FilterRegistry::generateFilterImpl()
    {
        // Get the id.
        constexpr id_type id = generateId<component_types...>();
        // Register the component types if it they aren't yet.
        Registry::registerComponentType<component_types...>();

        // Emplace filter info.
        filters().emplace_back(std::make_unique<filter_info<component_types...>>());

        auto& lists = entityLists();
        lists.emplace(id, hashed_sparse_set<entity>{});

        if (initialized())
        {
            // Check for any already existing entities that should be in this filter.
            for (auto& [entId, composition] : Registry::entityCompositions())
                if (filter_info<component_types...>{}.contains(composition))
                    lists.at(id).insert(entity{ &Registry::entityData(entId) });
        }

        return id;
    }

    template<typename... component_types>
    inline id_type FilterRegistry::generateFilter()
    {
        static const id_type id = generateFilterImpl<component_types...>();
        return id;
    }

}
