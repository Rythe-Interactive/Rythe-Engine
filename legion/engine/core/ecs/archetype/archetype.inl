#include <core/ecs/archetype/archetype.hpp>
#pragma once

namespace legion::core::ecs
{
    template<typename component_type, typename ...component_types>
    inline L_ALWAYS_INLINE archetype<component_type, component_types...>::archetype(const handleGroup& handles) noexcept : underlying(handles)
    {
    }

    template<typename component_type, typename ...component_types>
    inline L_ALWAYS_INLINE archetype<component_type, component_types...>::archetype(const component_type& comp, const component_types&... comps) noexcept : underlying(std::make_tuple(std::cref(comp), std::cref(comps)...))
    {
    }

    template<typename component_type, typename ...component_types>
    template<typename T>
    inline L_ALWAYS_INLINE T& archetype<component_type, component_types...>::get() noexcept
    {
        return std::get<component<T>>(std::get<handleGroup>(underlying));
    }

    template<typename component_type, typename ...component_types>
    inline L_ALWAYS_INLINE std::tuple<component_type&, component_types&...> archetype<component_type, component_types...>::get() noexcept
    {
        return std::make_tuple(std::ref(get<component_type>()), std::ref(get<component_types>())...);
    }

    template<typename component_type, typename ...component_types>
    template<std::size_t I>
    inline L_ALWAYS_INLINE element_at_t<I, component_type, component_types...>& archetype<component_type, component_types...>::get() noexcept
    {
        return std::get<I>(std::get<handleGroup>(underlying));
    }

    template<typename component_type, typename ...component_types>
    inline L_ALWAYS_INLINE bool archetype<component_type, component_types...>::valid() const noexcept
    {
        return std::apply([](auto&&... args) {return(args.valid() && ...); }, std::get<handleGroup>(underlying));
    }

    template<typename component_type, typename ...component_types>
    inline L_ALWAYS_INLINE archetype<component_type, component_types...>::operator bool() const noexcept
    {
        return valid();
    }

    template<typename component_type, typename ...component_types>
    inline L_ALWAYS_INLINE std::tuple<component_type&, component_types&...> archetype<component_type, component_types...>::create(entity ent)
    {
        return Registry::template createComponents<component_type, component_types...>(ent);
    }

    template<typename component_type, typename ...component_types>
    inline L_ALWAYS_INLINE std::tuple<component_type&, component_types&...> archetype<component_type, component_types...>::create(entity ent, component_type&& defaultValue, component_types&&... defaultValues)
    {
        return Registry::template createComponents<component_type, component_types...>(ent, std::forward<component_type>(defaultValue), std::forward<component_types>(defaultValues)...);
    }

    template<typename component_type, typename ...component_types>
    inline L_ALWAYS_INLINE std::tuple<component_type&, component_types&...> archetype<component_type, component_types...>::get(entity ent)
    {
        return Registry::template getComponents<component_type, component_types...>(ent);
    }

    template<typename component_type, typename ...component_types>
    inline L_ALWAYS_INLINE void archetype<component_type, component_types...>::destroy(entity ent)
    {
        Registry::template destroyComponents<component_type, component_types...>(ent);
    }

    template<typename component_type, typename ...component_types>
    inline L_ALWAYS_INLINE bool archetype<component_type, component_types...>::has(entity ent)
    {
        return Registry::template hasComponents<component_type, component_types...>(ent);
    }

}
