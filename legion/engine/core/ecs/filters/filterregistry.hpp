#pragma once
#include <unordered_map>

#include <core/containers/hashed_sparse_set.hpp>
#include <core/types/primitives.hpp>

#include <core/ecs/handles/entity.hpp>
#include <core/ecs/filters/filter_info.hpp>
#include <core/ecs/registry.hpp>

namespace legion::core::ecs
{
    class FilterRegistry
    {
        template<typename... component_types>
        friend struct filter;
    private:
        static std::unordered_map<id_type, hashed_sparse_set<entity>> m_entityLists;
        static std::vector<std::unique_ptr<filter_info_base>> m_filters;

        template<typename component_type>
        constexpr static id_type generateId()
        {
            return make_hash<component_type>();
        }

        template<typename component_type, typename... component_types>
        constexpr static id_type generateId()
        {
            return combine_hash(make_hash<component_type>(), generateId<component_types...>());
        }

        template<typename... component_types>
        static id_type generateFilter()
        {
            constexpr id_type id = generateId<component_types...>();
            m_filters.emplace_back(new filter_info<component_types...>{ id });
            m_entityLists.emplace(id, hashed_sparse_set<entity>{});

            for (auto& [ent, composition] : Registry::entityCompositions())
                if (filter_info<component_types...>{ id }.contains(composition))
                    m_entityLists.at(id).insert(ent);

            return id;
        }
    };
}
