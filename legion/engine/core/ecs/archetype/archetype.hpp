#pragma once
#include <tuple>
#include <variant>
#include <functional>
#include <type_traits>

#include <core/platform/platform.hpp>
#include <core/types/primitives.hpp>
#include <core/types/meta.hpp>

/**
 * @file archetype.hpp
 */

namespace legion::core::ecs
{
    template<typename component_type>
    struct component;

    struct entity;

    class Registry;

    /**@class archetype_base
     * @brief Common base class of all archetypes. Allows SFINAE checks for whether a templated type is an archetype.
     */
    struct archetype_base {};

    /**@class archetype
     * @brief API object for grouping multiple component types together.
     * @ref legion::core::transform
     */
    template<typename component_type, typename... component_types>
    struct archetype : public archetype_base
    {
        friend class legion::core::ecs::Registry;
        friend struct legion::core::ecs::entity;
    public:
        using handleGroup = std::tuple<component<component_type>, component<component_types>...>;
        using refGroup = std::tuple<component_type&, component_types&...>;
        using copyGroup = std::tuple<const component_type&, const component_types&...>;

        archetype() = default;
        archetype(const handleGroup& handles) noexcept;

        explicit archetype(const component_type& comp, const component_types&... comps) noexcept;

        /**@brief Get the handle to one of the components in the archetype.
         */
        template<typename T>
        L_NODISCARD T& get() noexcept;

        L_NODISCARD refGroup get() noexcept;

        template<std::size_t I>
        L_NODISCARD element_at_t<I, component_type, component_types...>& get() noexcept;

        L_NODISCARD bool valid() const noexcept;

        L_NODISCARD operator bool() const noexcept;

    private:
        std::variant<handleGroup, copyGroup> underlying;

        static refGroup create(entity ent);
        template<typename component_type0, typename... component_typeN>
        static refGroup create(entity ent, component_type0&& value0, component_typeN&&... valueN);
        static refGroup create(entity ent, archetype&& value);

        L_NODISCARD static refGroup get(entity ent);
        L_NODISCARD static handleGroup get_handles(entity ent);
        static void destroy(entity ent);
        L_NODISCARD static bool has(entity ent);
    };

    template<typename component_type, typename... component_types>
    using ar = archetype<component_type, component_types...>;

#if !defined(DOXY_EXCLUDE)
    template<typename component_type, typename... component_types>
    archetype(const std::tuple<component<component_type>, component<component_types>...>&) -> archetype<component_type, component_types...>;

    template<typename component_type, typename... component_types>
    archetype(const component_type&, const component_types&...) -> archetype<component_type, component_types...>;
#endif

    template <class T>
    struct ch_yield_type
    {
    };

    template <class T>
    struct ch_yield_type<component<T>>
    {
        using type = T;
    };

}

namespace std // NOLINT(cert-dcl58-cpp)
{
    template <::std::size_t I, class Arg, class... Args>
    struct tuple_element<I, legion::core::ecs::archetype<Arg, Args...>>
    {
        using type = typename legion::core::element_at_t<I, Arg, Args...>;
    };

    template <::std::size_t I, class... Args>
    tuple_element<I, legion::core::ecs::archetype<Args...>>
        get(legion::core::ecs::archetype<Args...>& val)
    {
        return val.template get<I>();
    }

    template <class X, class ... Args>
    legion::core::ecs::component<X> get(legion::core::ecs::archetype<Args...>& val)
    {
        return val.template get<X>();
    }

    template <class... Types>
    struct tuple_size<legion::core::ecs::archetype<Types...>>
        : public std::integral_constant<std::size_t, sizeof...(Types)>
    {
    };
    template <class... Types>
    struct tuple_size<const legion::core::ecs::archetype<Types...>>
        : public std::integral_constant<std::size_t, sizeof...(Types)>
    {
    };
    template <class... Types>
    struct tuple_size<volatile legion::core::ecs::archetype<Types...>>
        : public std::integral_constant<std::size_t, sizeof...(Types)>
    {
    };
    template <class... Types>
    struct tuple_size<const volatile legion::core::ecs::archetype<Types...>>
        : public std::integral_constant<std::size_t, sizeof...(Types)>
    {
    };

}
