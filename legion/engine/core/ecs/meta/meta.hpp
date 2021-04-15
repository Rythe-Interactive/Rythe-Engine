#pragma once
#include <type_traits>

#include <core/platform/platform.hpp>
#include <core/types/meta.hpp>

namespace legion::core::ecs
{
    struct component_base;

    template<typename component_type>
    struct component;

    struct entity;

    struct archetype_base;

    template<typename archetype_type>
    struct is_archetype : std::conditional_t<std::is_base_of_v<archetype_base, remove_cvr_t<archetype_type>>, std::true_type, std::false_type> {};

    template<typename archetype_type>
    constexpr bool is_archetype_v = is_archetype<archetype_type>::value;

    template<typename component_type>
    struct maybe_component : std::conditional_t<is_archetype_v<component_type>, std::false_type, std::true_type> {};

    template<typename component_type>
    constexpr bool maybe_component_v = maybe_component<component_type>::value;

    namespace detail
    {
        template<bool,typename>
        struct _wrap_component_impl;

        template<typename component_type>
        struct _wrap_component_impl<true, component_type>
        {
            using type = component<remove_cvr_t<component_type>>;
        };

        template<typename archetype_type>
        struct _wrap_component_impl<false, archetype_type>
        {
            using type = typename archetype_type::handleGroup;
        };
    }

    template<typename component_type>
    struct wrap_component
    {
        using type = typename detail::_wrap_component_impl<maybe_component_v<component_type>, component_type>::type;
    };

    template<typename component_type>
    using wrap_component_t = typename wrap_component<component_type>::type;
}
