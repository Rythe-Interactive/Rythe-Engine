#pragma once
#include <array>

#include <core/ecs/filters/filterregistry.hpp>

namespace legion::core::ecs
{
    struct filter_base
    {

    };

    template<typename... component_types>
    struct filter : public filter_base
    {
        static const id_type id = FilterRegistry::generateFilter<component_types...>();
        static const std::array<id_type, sizeof...(component_types)> composition = { typeHash<component_types>()... };
    };
}
