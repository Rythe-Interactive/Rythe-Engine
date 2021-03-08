#include <core/ecs/registry.hpp>
#pragma once

namespace legion::core::ecs
{
    template<typename component_type>
    inline void FilterRegistry::markComponentAdd(entity target)
    {
        auto& composition = Registry::entityComposition(target);

        for (auto& filter : m_filters)
            if (filter->contains(make_hash<component_type>()) && filter->contains(composition))
                m_entityLists.at(filter->id).insert(target);
    }

    template<typename component_type>
    inline void FilterRegistry::markComponentErase(entity target)
    {
        auto& composition = Registry::entityComposition(target);

        for (auto& filter : m_filters)
            if (filter->contains(make_hash<component_type>()))
                m_entityLists.at(filter->id).erase(target); // Will not do anything if the target wasn't in the set.
    }

    template<typename component_type>
    inline constexpr id_type FilterRegistry::generateId() noexcept
    {
        return make_hash<component_type>();
    }

    template<typename component_type, typename... component_types>
    inline constexpr id_type FilterRegistry::generateId() noexcept
    {
        return combine_hash(make_hash<component_type>(), generateId<component_types...>());
    }

    template<typename ...component_types>
    inline id_type FilterRegistry::generateFilterImpl()
    {
        constexpr id_type id = generateId<component_types...>();
        m_filters.emplace_back(new filter_info<component_types...>{ id });
        m_entityLists.emplace(id, hashed_sparse_set<entity>{});

        for (auto& [ent, composition] : Registry::entityCompositions())
            if (filter_info<component_types...>{ id }.contains(composition))
                m_entityLists.at(id).insert(ent);

        return id;
    }

    template<typename ...component_types>
    inline id_type FilterRegistry::generateFilter()
    {
        static id_type id = generateFilterImpl<component_types...>();
        return id;
    }

}
