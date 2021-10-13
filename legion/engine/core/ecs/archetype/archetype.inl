#include <core/ecs/archetype/archetype.hpp>
#pragma once

namespace legion::core::ecs
{
    template<typename component_type, typename... component_types>
    inline archetype<component_type, component_types...>::archetype(const archetype& src) noexcept : owner(src.owner), underlying(src.underlying)
    {
    }

    template<typename component_type, typename... component_types>
    inline archetype<component_type, component_types...>::archetype(archetype&& src) noexcept : owner(std::move(src.owner)), underlying(std::move(src.underlying))
    {
    }

    template<typename component_type, typename... component_types>
    inline L_ALWAYS_INLINE archetype<component_type, component_types...>::archetype(const handle_group& handles) noexcept : owner(std::get<0>(handles).owner), underlying(handles)
    {
    }

    template<typename component_type, typename... component_types>
    inline L_ALWAYS_INLINE archetype<component_type, component_types...>::archetype(const component_type& comp, const component_types&... comps) noexcept : owner(), underlying(std::make_tuple(std::cref(comp), std::cref(comps)...))
    {
    }

    template<typename component_type, typename... component_types>
    inline archetype<component_type, component_types...>& archetype<component_type, component_types...>::operator=(const archetype& src)
    {
        owner = src.owner;
        if (std::holds_alternative<copy_group>(src.underlying))
        {
            auto& copySrc = std::get<copy_group>(src.underlying);
            underlying = std::make_tuple(std::cref(std::get<const component_type&>(copySrc)), std::cref(std::get<const component_types&>(copySrc))...);
        }
        else if (std::holds_alternative<handle_group>(src.underlying))
        {
            underlying = std::get<handle_group>(src.underlying);
        }
        return *this;
    }

    template<typename component_type, typename... component_types>
    inline archetype<component_type, component_types...>& archetype<component_type, component_types...>::operator=(archetype&& src)
    {
        owner = std::move(src.owner);
        underlying = std::move(std::get<0>(src.underlying));
        return *this;
    }

    template<typename component_type, typename... component_types>
    template<typename T>
    inline L_ALWAYS_INLINE T& archetype<component_type, component_types...>::get()
    {
        return std::get<component<T>>(std::get<handle_group>(underlying));
    }

    template<typename component_type, typename... component_types>
    inline L_ALWAYS_INLINE std::tuple<component_type&, component_types&...> archetype<component_type, component_types...>::get()
    {
        return std::make_tuple(std::ref(get<component_type>()), std::ref(get<component_types>())...);
    }

    template<typename component_type, typename... component_types>
    template<std::size_t I>
    inline L_ALWAYS_INLINE element_at_t<I, component_type, component_types...>& archetype<component_type, component_types...>::get()
    {
        return std::get<I>(std::get<handle_group>(underlying));
    }

    template<typename component_type, typename... component_types>
    inline L_ALWAYS_INLINE std::tuple<component<component_type>, component<component_types>...> archetype<component_type, component_types...>::handles()
    {
        return std::get<handle_group>(underlying);
    }

    template<typename component_type, typename... component_types>
    inline L_ALWAYS_INLINE std::tuple<const component<component_type>, const component<component_types>...>  archetype<component_type, component_types...>::handles() const
    {
        auto& handles = std::get<handle_group>(underlying);
        return std::make_tuple(const_cast<const component<component_type>>(std::get<component<component_type>>(handles)), const_cast<const component<component_types>>(std::get<component<component_types>>(handles))...);
    }

    template<typename component_type, typename... component_types>
    inline L_ALWAYS_INLINE std::tuple<component_type&, component_types&...> archetype<component_type, component_types...>::values()
    {
        return get();
    }

    template<typename component_type, typename... component_types>
    inline L_ALWAYS_INLINE bool archetype<component_type, component_types...>::valid() const
    {
        return std::apply([](auto&&... args) {return(args.valid() && ...); }, std::get<handle_group>(underlying));
    }

    template<typename component_type, typename... component_types>
    inline L_ALWAYS_INLINE archetype<component_type, component_types...>::operator bool() const
    {
        return valid();
    }

    template<typename component_type, typename... component_types>
    inline void archetype<component_type, component_types...>::destroy()
    {
        Registry::template destroyComponent<component_type, component_types...>(owner);
    }

    template<typename component_type, typename... component_types>
    inline L_ALWAYS_INLINE std::tuple<component_type&, component_types&...> archetype<component_type, component_types...>::create(entity ent)
    {
        return Registry::template createComponent<component_type, component_types...>(ent);
    }

    template<typename component_type, typename... component_types>
    template<typename component_type0, typename... component_typeN>
    inline L_ALWAYS_INLINE std::tuple<component_type&, component_types&...> archetype<component_type, component_types...>::create(entity ent, component_type0&& value, component_typeN&&... valueN)
    {
        static_assert(std::is_convertible_v<remove_cvr_t<component_type>, remove_cvr_t<component_type0>> && (std::is_convertible_v<remove_cvr_t<component_types>, remove_cvr_t<component_typeN>> && ...), "Component values need to be the same as the ones of the archetype.");
        return Registry::template createComponent<component_type, component_types...>(ent, std::forward<component_type0>(value), std::forward<component_typeN>(valueN)...);
    }

    template<typename component_type, typename... component_types>
    inline L_ALWAYS_INLINE std::tuple<component_type&, component_types&...> archetype<component_type, component_types...>::create(entity ent, archetype&& value)
    {
        return Registry::template createComponent<component_type, component_types...>(ent, std::move(std::get<component_type>(std::get<copy_group>(value.underlying))), std::move(std::get<component_types>(std::get<copy_group>(value.underlying)))...);
    }

    template<typename component_type, typename... component_types>
    inline L_ALWAYS_INLINE std::tuple<component_type&, component_types&...> archetype<component_type, component_types...>::get(entity ent)
    {
        return Registry::template getComponent<component_type, component_types...>(ent);
    }

    template<typename component_type, typename... component_types>
    inline L_ALWAYS_INLINE std::tuple<component<component_type>, component<component_types>...> archetype<component_type, component_types...>::get_handles(entity ent)
    {
        return std::make_tuple(component<component_type>{ {}, ent }, component<component_types>{ {}, ent }...);
    }

    template<typename component_type, typename... component_types>
    inline L_ALWAYS_INLINE std::tuple<const component<component_type>, const component<component_types>...> archetype<component_type, component_types...>::get_const_handles(entity ent)
    {
        return std::make_tuple(const_cast<const component<component_type>>(component<component_type>{ {}, ent }), const_cast<const component<component_types>>(component<component_types>{ {}, ent })...);
    }

    template<typename component_type, typename... component_types>
    inline L_ALWAYS_INLINE void archetype<component_type, component_types...>::destroy(entity ent)
    {
        Registry::template destroyComponent<component_type, component_types...>(ent);
    }

    template<typename component_type, typename... component_types>
    inline L_ALWAYS_INLINE bool archetype<component_type, component_types...>::has(entity ent)
    {
        return Registry::template hasComponent<component_type, component_types...>(ent);
    }

}
