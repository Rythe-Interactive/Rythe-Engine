#include <core/ecs/archetype/archetype.hpp>
#pragma once

namespace legion::core::ecs
{
    template<typename component_type, typename... component_types>
    inline L_ALWAYS_INLINE archetype<component_type, component_types...>::archetype(const handleGroup& handles) noexcept : underlying(handles)
    {
    }

    template<typename component_type, typename... component_types>
    inline L_ALWAYS_INLINE archetype<component_type, component_types...>::archetype(const component_type& comp, const component_types&... comps) noexcept : underlying(std::make_tuple(std::cref(comp), std::cref(comps)...))
    {
    }

    template<typename component_type, typename... component_types>
    template<typename T>
    inline L_ALWAYS_INLINE T& archetype<component_type, component_types...>::get() noexcept
    {
        return std::get<component<T>>(std::get<handleGroup>(underlying));
    }

    template<typename component_type, typename... component_types>
    inline L_ALWAYS_INLINE std::tuple<component_type&, component_types&...> archetype<component_type, component_types...>::get() noexcept
    {
        return std::make_tuple(std::ref(get<component_type>()), std::ref(get<component_types>())...);
    }

    template<typename component_type, typename... component_types>
    template<std::size_t I>
    inline L_ALWAYS_INLINE element_at_t<I, component_type, component_types...>& archetype<component_type, component_types...>::get() noexcept
    {
        return std::get<I>(std::get<handleGroup>(underlying));
    }

    template<typename component_type, typename... component_types>
    inline L_ALWAYS_INLINE bool archetype<component_type, component_types...>::valid() const noexcept
    {
        return std::apply([](auto&&... args) {return(args.valid() && ...); }, std::get<handleGroup>(underlying));
    }

    template<typename component_type, typename... component_types>
    inline L_ALWAYS_INLINE archetype<component_type, component_types...>::operator bool() const noexcept
    {
        return valid();
    }

    template<typename component_type, typename... component_types>
    inline L_ALWAYS_INLINE std::tuple<component_type&, component_types&...> archetype<component_type, component_types...>::create(entity ent)
    {
        return Registry::template createComponents<component_type, component_types...>(ent);
    }

    template<typename component_type, typename... component_types>
    template<typename component_type0, typename... component_typeN>
    inline L_ALWAYS_INLINE std::tuple<component_type&, component_types&...> archetype<component_type, component_types...>::create(entity ent, component_type0&& value, component_typeN&&... valueN)
    {
        static_assert(std::is_convertible_v<remove_cvr_t<component_type>, remove_cvr_t<component_type0>> && (std::is_convertible_v<remove_cvr_t<component_types>, remove_cvr_t<component_typeN>> && ...), "Component values need to be the same as the ones of the archetype.");
        return Registry::template createComponents<component_type, component_types...>(ent, std::forward<component_type0>(value), std::forward<component_typeN>(valueN)...);
    }

    template<typename component_type, typename... component_types>
    inline L_ALWAYS_INLINE std::tuple<component_type&, component_types&...> archetype<component_type, component_types...>::create(entity ent, archetype&& value)
    {
        return Registry::template createComponents<component_type, component_types...>(ent, std::move(std::get<component_type>(std::get<1>(value.underlying))), std::move(std::get<component_types>(std::get<1>(value.underlying)))...);
    }

    template<typename component_type, typename... component_types>
    inline L_ALWAYS_INLINE std::tuple<component_type&, component_types&...> archetype<component_type, component_types...>::get(entity ent)
    {
        return Registry::template getComponents<component_type, component_types...>(ent);
    }

    template<typename component_type, typename... component_types>
    inline L_ALWAYS_INLINE void archetype<component_type, component_types...>::destroy(entity ent)
    {
        Registry::template destroyComponents<component_type, component_types...>(ent);
    }

    template<typename component_type, typename... component_types>
    inline L_ALWAYS_INLINE bool archetype<component_type, component_types...>::has(entity ent)
    {
        return Registry::template hasComponents<component_type, component_types...>(ent);
    }

}
