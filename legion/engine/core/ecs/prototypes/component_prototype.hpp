#pragma once
#include <unordered_set>

#include <core/serialization/prototype.hpp>
#include <core/serialization/reflector.hpp>

#include <core/ecs/handles/component.hpp>

namespace legion::core::serialization
{
    template<>
    struct prototype<ecs::component_base> : public prototype_base
    {
    };

    template<typename component_type>
    struct prototype<ecs::component<component_type>> : public prototype<ecs::component_base>, public reflector<component_type>
    {        
    };

    using component_prototype_base = prototype<ecs::component_base>;

    template<typename component_type>
    using component_prototype = prototype<ecs::component<component_type>>;
}
