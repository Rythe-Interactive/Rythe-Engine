#pragma once
#include <unordered_map>

#include <core/containers/hashed_sparse_set.hpp>
#include <core/types/primitives.hpp>

#include <core/ecs/handles/entity.hpp>
#include <core/ecs/handles/filter.hpp>
#include <core/ecs/filters/filter_info.hpp>

namespace legion::core::ecs
{
    class FilterRegistry
    {
    public:
        template<typename... component_types>
        friend struct filter;
        template<typename... component_types>
        friend struct filter_info;

        static void markComponentAdd(id_type componentId, entity target);

        static void markComponentErase(id_type componentId, entity target);

        template<typename component_type>
        static void markComponentAdd(entity target);

        template<typename component_type>
        static void markComponentErase(entity target);

        static void markEntityDestruction(entity target);

        static void markEntityFullCreation(entity target);

        static entity_set& getList(id_type filterId);

    private:
        static std::unordered_map<id_type, entity_set> m_entityLists;
        static std::vector<std::unique_ptr<filter_info_base>> m_filters;

        template<typename component_type>
        constexpr static id_type generateId() noexcept;

        template<typename component_type0, typename component_type1, typename... component_types>
        constexpr static id_type generateId() noexcept;

        template<typename... component_types>
        static const id_type generateFilterImpl();

        template<typename... component_types>
        static const id_type generateFilter();
    };
}

#include <core/ecs/filters/filter_info.inl>
#include <core/ecs/handles/filter.inl>
