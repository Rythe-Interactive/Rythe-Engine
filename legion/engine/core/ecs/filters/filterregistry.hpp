#pragma once
#include <unordered_map>

#include <core/containers/hashed_sparse_set.hpp>
#include <core/types/primitives.hpp>

#include <core/ecs/handles/entity.hpp>

namespace legion::core::ecs
{
    class FilterRegistry
    {
        template<typename... component_types>
        friend struct filter;
    private:
        static std::unordered_map<id_type, hashed_sparse_set<entity>> m_entityLists;

        template<typename... component_types>
        static id_type generateFilter();
    };
}
