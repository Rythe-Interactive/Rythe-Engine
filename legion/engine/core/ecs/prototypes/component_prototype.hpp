#pragma once
#include <unordered_set>

#include <core/serialization/prototype.hpp>
#include <core/types/reflector.hpp>

#include <core/ecs/handles/component.hpp>

namespace legion::core::serialization
{
    template<>
    struct prototype<ecs::component_base> : public prototype_base
    {
    };

    template<typename component_type>
    struct prototype<ecs::component<component_type>> : public prototype<ecs::component_base>, public decltype(make_reflector(std::declval<component_type>()))
    {
        using Reflector = decltype(make_reflector(std::declval<component_type>()));

        prototype() = default;
        prototype(const component_type& src) : Reflector(make_reflector(src);) {}
        prototype(component_type&& src) : Reflector(make_reflector(src);) {}
    };

    using component_prototype_base = prototype<ecs::component_base>;

    template<typename component_type>
    using component_prototype = prototype<ecs::component<component_type>>;
}
