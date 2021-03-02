#pragma once
#include <unordered_map>

#include <core/containers/hashed_sparse_set.hpp>
#include <core/types/primitives.hpp>

#include <core/ecs/handles/entity.hpp>
#include <core/ecs/filters/filter_info.hpp>

namespace legion::core::ecs
{
    class FilterRegistry
    {
    public:
        template<typename... component_types>
        friend struct filter;

        template<typename component_type>
        static void markComponentAdd(entity target);

        template<typename component_type>
        static void markComponentErase(entity target);

        static void markEntityDestruction(entity target);

    private:
        static std::unordered_map<id_type, hashed_sparse_set<entity>> m_entityLists;
        static std::vector<std::unique_ptr<filter_info_base>> m_filters;

        template<typename component_type>
        constexpr static id_type generateId() noexcept;

        template<typename component_type, typename... component_types>
        constexpr static id_type generateId() noexcept;

        template<typename... component_types>
        static id_type generateFilterImpl();

        template<typename... component_types>
        static id_type generateFilter();
    };
}
