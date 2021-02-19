#pragma once
#include <type_traits>

#include <core/types/meta.hpp>
#include <core/types/primitives.hpp>

namespace legion::core::serialization
{
    namespace detail
    {
        struct any_type {
            template<class T>
            constexpr operator T(); // non explicit
        };
    }

    template<typename T, typename... MemberTypes>
    struct reflector
    {
        using source_type = T;

        inline static constexpr size_type size = sizeof...(MemberTypes);
        std::tuple<MemberTypes...> values;
        std::array<std::string, sizeof...(MemberTypes)> names;

        reflector() = default;
        template<typename Type, typename... Members>
        reflector(Type&&, std::array<std::string, sizeof...(MemberTypes)>&& memberNames, Members&&... members)
            : values(std::forward<Members>(members)...), names(memberNames) {}
    };

    template<typename T, typename... MemberTypes>
    reflector(T&&, std::array<std::string, sizeof...(MemberTypes)>&&, MemberTypes&&...)
        ->reflector<std::remove_cv_t<std::remove_reference_t<T>>, std::remove_cv_t<std::remove_reference_t<MemberTypes>>...>;

    template<typename T>
    struct reflector<T>
    {
        using source_type = T;

        inline static constexpr std::size_t size = 0;
        std::tuple<> values;
        std::array<std::string, 0> names;
    };

    template<typename T>
    auto make_reflector(T&& object)
    {
        using type = std::remove_cv_t<std::remove_reference_t<T>>;

        if constexpr (is_brace_constructible_v<reflector<type>, T>)
        {
            return reflector<type>{ object };
        }
        else if constexpr (is_brace_constructible_v<type,
            detail::any_type, detail::any_type, detail::any_type, detail::any_type,
            detail::any_type, detail::any_type, detail::any_type, detail::any_type>) {
            auto&& [p0, p1, p2, p3, p4, p5, p6, p7] = std::forward<T>(object);
            return reflector(std::forward<T>(object),
                { "value0", "value1", "value2", "value3", "value4", "value5", "value6", "value7" },
                p0, p1, p2, p3, p4, p5, p6, p7);
        }
        else if constexpr (is_brace_constructible_v<type,
            detail::any_type, detail::any_type, detail::any_type, detail::any_type,
            detail::any_type, detail::any_type, detail::any_type>) {
            auto&& [p0, p1, p2, p3, p4, p5, p6] = std::forward<T>(object);
            return reflector(std::forward<T>(object),
                { "value0", "value1", "value2", "value3", "value4", "value5", "value6" },
                p0, p1, p2, p3, p4, p5, p6);
        }
        else if constexpr (is_brace_constructible_v<type,
            detail::any_type, detail::any_type, detail::any_type, detail::any_type,
            detail::any_type, detail::any_type>) {
            auto&& [p0, p1, p2, p3, p4, p5] = std::forward<T>(object);
            return reflector(std::forward<T>(object),
                { "value0", "value1", "value2", "value3", "value4", "value5" },
                p0, p1, p2, p3, p4, p5);
        }
        else if constexpr (is_brace_constructible_v<type,
            detail::any_type, detail::any_type, detail::any_type, detail::any_type, detail::any_type>) {
            auto&& [p0, p1, p2, p3, p4] = std::forward<T>(object);
            return reflector(std::forward<T>(object),
                { "value0", "value1", "value2", "value3", "value4" }, p0, p1, p2, p3, p4);
        }
        else if constexpr (is_brace_constructible_v<type,
            detail::any_type, detail::any_type, detail::any_type, detail::any_type>) {
            auto&& [p0, p1, p2, p3] = std::forward<T>(object);
            return reflector(std::forward<T>(object),
                { "value0", "value1", "value2", "value3" }, p0, p1, p2, p3);
        }
        else if constexpr (is_brace_constructible_v<type,
            detail::any_type, detail::any_type, detail::any_type>) {
            auto&& [p0, p1, p2] = std::forward<T>(object);
            return reflector(std::forward<T>(object), { "value0", "value1", "value2" }, p0, p1, p2);
        }
        else if constexpr (is_brace_constructible_v<type, detail::any_type, detail::any_type>) {
            auto&& [p0, p1] = std::forward<T>(object);
            return reflector(std::forward<T>(object), { "value0", "value1" }, p0, p1);
        }
        else if constexpr (is_brace_constructible_v<type, detail::any_type>) {
            auto&& [p0] = std::forward<T>(object);
            return reflector(std::forward<T>(object), { "value0" }, p0);
        }
        else {
            return reflector<type>();
        }
    }

    namespace detail
    {
        template <typename source_type, typename reflector_type, std::size_t... I>
        constexpr source_type make_from_reflector_impl(reflector_type&& r, std::index_sequence<I...>)
        {
            return source_type{ std::get<I>(r.values)... };
        }
    }

    template<typename reflector_type>
    auto from_reflector(reflector_type&& r)
    {
        return detail::make_from_reflector_impl<typename std::remove_reference_t<reflector_type>::source_type>(
            std::forward<reflector_type>(r), std::make_index_sequence<std::remove_reference_t<reflector_type>::size>{});
    }
}
