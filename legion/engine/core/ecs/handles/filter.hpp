#pragma once
#include <core/ecs/filters/filterregistry.hpp>

namespace legion::core::ecs
{
    template<typename... component_types>
    struct filter
    {
        static const id_type id = FilterRegistry::generateFilter<component_types...>();
    };
}
