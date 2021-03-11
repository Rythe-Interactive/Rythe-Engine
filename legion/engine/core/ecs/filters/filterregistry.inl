#include <core/ecs/registry.hpp>
#pragma once

namespace legion::core::ecs
{
    template<typename component_type>
    inline void FilterRegistry::markComponentAdd(entity target)
    {
        auto& composition = Registry::entityComposition(target);

        for (auto& filter : filters())
            if (filter->contains(make_hash<component_type>()) && filter->contains(composition))
                entityLists().at(filter->id()).insert(target);
    }

    template<typename component_type>
    inline void FilterRegistry::markComponentErase(entity target)
    {
        auto& composition = Registry::entityComposition(target);

        for (auto& filter : filters())
            if (filter->contains(make_hash<component_type>()))
                entityLists().at(filter->id()).erase(target); // Will not do anything if the target wasn't in the set.
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

    template<typename ...component_types>
    inline const id_type FilterRegistry::generateFilterImpl()
    {
        constexpr id_type id = generateId<component_types...>();
        filters().emplace_back(new filter_info<component_types...>());
        entityLists().emplace(id, hashed_sparse_set<entity>{});

        for (auto& [entId, composition] : Registry::entityCompositions())
            if (filter_info<component_types...>{}.contains(composition))
                entityLists().at(id).insert(entity{ &Registry::entityData(entId) });

        return id;
    }

    template<typename ...component_types>
    inline const id_type FilterRegistry::generateFilter()
    {
        static const id_type id = generateFilterImpl<component_types...>();
        return id;
    }

}
