#pragma once
#include <type_traits>
#include <tuple>
#include <array>
#include <functional>

#include <core/types/meta.hpp>
#include <core/types/primitives.hpp>
#include <core/platform/platform.hpp>

namespace legion::core
{
    namespace detail
    {
        /**@struct any_type
         * @brief placeholder type that can be used to detect brace initializability.
         */
        struct any_type {
            template<class T>
            constexpr operator T(); // implicit conversion to any type.
        };
    }

    /**@struct reflector
     * @brief Struct that allows the use of static reflection on arbitrary types. Can be custom specialized for more accurate reflection.
     */
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

#if !defined(DOXY_EXCLUDE)
    template<typename T, typename... MemberTypes>
    reflector(T&&, std::array<std::string, sizeof...(MemberTypes)>&&, MemberTypes&&...)
        ->reflector<remove_cvr_t<T>, remove_cvr_t<MemberTypes>...>;
#endif

    /**@brief Template specialization for empty classes and unreflectable types.
     */
    template<typename T>
    struct reflector<T>
    {
        using source_type = T;

        inline static constexpr size_type size = 0;
        std::tuple<> values;
        std::array<std::string, 0> names;
    };

    namespace detail
    {
        template<template<typename...>typename T, typename U, size_type I, typename... Args>
        struct make_sequence : make_sequence<T, U, I - 1, Args..., U> {};

        template<template<typename...>typename T, typename U, typename... Args>
        struct make_sequence<T, U, 0, Args...>
        {
            using type = T<Args...>;
        };

        template<template<typename...>typename T, typename U, size_type I, typename... Args>
        using make_sequence_t = typename make_sequence<T, U, I, Args...>::type;
    }

    // Reflector return code
#define RETURN_REFLECTOR(...)                                                       \
    auto&& [__VA_ARGS__] = std::forward<T>(object);                                 \
    return reflector(std::forward<T>(object), { STRINGIFY_SEPERATE(__VA_ARGS__) }, __VA_ARGS__);

    /**@brief Create reflector of a certain value.
     * @param object Object to reflect.
     * @return
     */
    template<typename T>
    auto make_reflector(T&& object)
    {
        using type = remove_cvr_t<T>;

        // Check for a custom specialization.
        if constexpr (is_brace_constructible_v<reflector<type>, T>)
        {
            // Return specialized reflector.
            return reflector<type>{ object };
        }
        // Check for construction with 16 items.
        else if constexpr (detail::make_sequence_t<is_brace_constructible, detail::any_type, 16, type>::value) {
            RETURN_REFLECTOR(value0, value1, value2, value3, value4, value5, value6, value7, value8, value9, value10, value11, value12, value13, value14, value16);
        }
        // Check for construction with 15 items.
        else if constexpr (detail::make_sequence_t<is_brace_constructible, detail::any_type, 15, type>::value) {
            RETURN_REFLECTOR(value0, value1, value2, value3, value4, value5, value6, value7, value8, value9, value10, value11, value12, value13, value14);
        }
        // Check for construction with 14 items.
        else if constexpr (detail::make_sequence_t<is_brace_constructible, detail::any_type, 14, type>::value) {
            RETURN_REFLECTOR(value0, value1, value2, value3, value4, value5, value6, value7, value8, value9, value10, value11, value12, value13);
        }
        // Check for construction with 13 items.
        else if constexpr (detail::make_sequence_t<is_brace_constructible, detail::any_type, 13, type>::value) {
            RETURN_REFLECTOR(value0, value1, value2, value3, value4, value5, value6, value7, value8, value9, value10, value11, value12);
        }
        // Check for construction with 12 items.
        else if constexpr (detail::make_sequence_t<is_brace_constructible, detail::any_type, 12, type>::value) {
            RETURN_REFLECTOR(value0, value1, value2, value3, value4, value5, value6, value7, value8, value9, value10, value11);
        }
        // Check for construction with 11 items.
        else if constexpr (detail::make_sequence_t<is_brace_constructible, detail::any_type, 11, type>::value) {
            RETURN_REFLECTOR(value0, value1, value2, value3, value4, value5, value6, value7, value8, value9, value10);
        }
        // Check for construction with 10 items.
        else if constexpr (detail::make_sequence_t<is_brace_constructible, detail::any_type, 10, type>::value) {
            RETURN_REFLECTOR(value0, value1, value2, value3, value4, value5, value6, value7, value8, value9);
        }
        // Check for construction with 9 items.
        else if constexpr (detail::make_sequence_t<is_brace_constructible, detail::any_type, 9, type>::value) {
            RETURN_REFLECTOR(value0, value1, value2, value3, value4, value5, value6, value7, value8);
        }
        // Check for construction with 8 items.
        else if constexpr (detail::make_sequence_t<is_brace_constructible, detail::any_type, 8, type>::value) {
            RETURN_REFLECTOR(value0, value1, value2, value3, value4, value5, value6, value7);
        }
        // Check for construction with 7 items.
        else if constexpr (detail::make_sequence_t<is_brace_constructible, detail::any_type, 7, type>::value) {
            RETURN_REFLECTOR(value0, value1, value2, value3, value4, value5, value6);
        }
        // Check for construction with 6 items.
        else if constexpr (detail::make_sequence_t<is_brace_constructible, detail::any_type, 6, type>::value) {
            RETURN_REFLECTOR(value0, value1, value2, value3, value4, value5);
        }
        // Check for construction with 5 items.
        else if constexpr (detail::make_sequence_t<is_brace_constructible, detail::any_type, 5, type>::value) {
            RETURN_REFLECTOR(value0, value1, value2, value3, value4);
        }
        // Check for construction with 4 items.
        else if constexpr (detail::make_sequence_t<is_brace_constructible, detail::any_type, 4, type>::value) {
            RETURN_REFLECTOR(value0, value1, value2, value3);
        }
        // Check for construction with 3 items.
        else if constexpr (detail::make_sequence_t<is_brace_constructible, detail::any_type, 3, type>::value) {
            RETURN_REFLECTOR(value0, value1, value2);
        }
        // Check for construction with 2 items.
        else if constexpr (is_brace_constructible_v<type, detail::any_type, detail::any_type>) {
            RETURN_REFLECTOR(value0, value1);
        }
        // Check for construction with 1 items.
        else if constexpr (is_brace_constructible_v<type, detail::any_type>) {
            RETURN_REFLECTOR(value0);
        }

        // It's either an empty class or it has more variables than we have implementation for.
        else
        {
            return reflector<type>();
        }
    }

#undef RETURN_REFLECTOR

    namespace detail
    {
        template <typename SRC, typename Reflector, size_type... I>
        constexpr SRC make_from_reflector_impl(Reflector&& r, std::index_sequence<I...>)
        {
            return SRC{ std::get<I>(r.values)... };
        }
    }

    template<typename Reflector>
    auto from_reflector(Reflector&& r)
    {
        return detail::make_from_reflector_impl<typename std::remove_reference_t<Reflector>::source_type>(
            std::forward<Reflector>(r), std::make_index_sequence<std::remove_reference_t<Reflector>::size>{});
    }

    template <typename T, typename Reflector>
    struct variable_index;

    template <typename T, typename SRC, typename... Types>
    struct variable_index<T, reflector<SRC, T, Types...>> {
        static const size_type value = 0;
    };

    template <typename T, typename SRC, typename U, typename... Types>
    struct variable_index<T, reflector<SRC, U, Types...>> {
        static const size_type value = 1 + variable_index<T, reflector<SRC, Types...>>::value;
    };

    template <typename T, typename Reflector>
    inline constexpr size_type variable_index_v = variable_index<T, Reflector>::value;

    template<size_type I, typename Reflector>
    struct reflector_element;

    template <size_type I, typename T, typename ... Types>
    struct reflector_element<I, reflector<T, Types...>>
    {
        using type = std::pair<std::string&, typename std::remove_reference_t<typename std::tuple_element<I, std::tuple<Types...>>::type>&>;
    };

    template <::std::size_t I, typename T, typename ... Types>
    reflector_element<I, reflector<T, Types...>>
        get(reflector<T, Types...>& val)
    {
        return std::make_pair(std::ref(val.names[I]), std::ref(std::get<I>(val.values)));
    }

    template <typename X, typename T, typename ... Types>
    reflector_element<variable_index_v<X, reflector<T, Types...>>, reflector<T, Types...>>
        get(reflector<T, Types...>& val)
    {
        return get<variable_index_v<X, reflector<T, Types...>>>(val);
    }

    namespace detail
    {
        template<template<typename>typename Compare, typename T, T A, T B>
        struct compare
        {
            static constexpr inline Compare<T> comp{};
            static constexpr inline bool value = comp(A, B);
        };

        template<template<typename>typename Compare, typename T, T A, T B>
        inline constexpr bool compare_v = compare<Compare, T, A, B>::value;

        template<std::size_t I, typename Reflector, typename Func>
        typename std::enable_if_t<I == std::remove_reference_t<Reflector>::size, void>
            for_each_impl(Reflector&& r, Func&& f) {};

        template<std::size_t I, typename Reflector, typename Func>
        typename std::enable_if_t < I < std::remove_reference_t<Reflector>::size, void>
            for_each_impl(Reflector&& r, Func&& f)
        {
            std::invoke(std::forward<Func>(f), r.names[I], std::get<I>(r.values));
            for_each_impl<I + 1>(std::forward<Reflector>(r), std::forward<Func>(f));
        };

        template<int I, template<typename> typename Compare, int End, int Increment, typename Reflector, typename Func>
        typename std::enable_if_t<!compare_v<Compare, int, I, End>, void>
            for_i_impl(Reflector&& r, Func&& f) {};

        template<int I, template<typename> typename Compare, int End, int Increment, typename Reflector, typename Func>
        typename std::enable_if_t<compare_v<Compare, int, I, End>, void>
            for_i_impl(Reflector&& r, Func&& f)
        {
            std::invoke(std::forward<Func>(f), r.names[I], std::get<I>(r.values));
            for_i_impl<I + Increment, Compare, End, Increment>(std::forward<Reflector>(r), std::forward<Func>(f));
        };
    }

    /**@brief For each loop iteration over all items in a reflector.
     * @param r Reflector to operate on.
     * @param f Functor to invoke at each iteration. Needs to be able to take a string as the first parameter, and a generic type as the second parameter.
     * Example:
     * @code{.cpp}
     *  for_each(refl, [](std::string& name, auto& val) { print(name, val); });
     * @endcode
     */
    template<typename Reflector, typename Func>
    void for_each(Reflector&& r, Func&& f)
    {
        detail::for_each_impl<0>(std::forward<Reflector>(r), std::forward<Func>(f));
    }

    /**@brief For loop iteration over items in a reflector according to an iteration pattern similar to: `int i = I; Compare(i, End); i += Increment`
     * @tparam I Start iteration value.
     * @tparam Compare Comparator class, iteration will stop when it returns false, std::less and std::greater are good examples.
     *         operator() needs to be constexpr.
     * @tparam End Value to compare iteration index with to signal end of iteration.
     * @tparam Increment Value to increment iteration index with.
     * @param r Reflector to operate on.
     * @param f Functor to invoke at each iteration. Needs to be able to take a string as the first parameter, and a generic type as the second parameter.
     *
     * Example:
     * @code{.cpp}
     *  for_i<0, std::less, refl.size, 1>(refl, [](std::string& name, auto& val) { print(name, val); });
     * @endcode
     */
    template<int I, template<typename> typename Compare, int End, int Increment, typename Reflector, typename Func>
    void for_i(Reflector&& r, Func&& f)
    {
        detail::for_i_impl<I, Compare, End, Increment>(std::forward<Reflector>(r), std::forward<Func>(f));
    }

    /**@brief For loop iteration over items in a reflector according to an iteration pattern similar to: `int i = I; i < End; i += Increment`
     * @tparam I Start iteration value.
     * @tparam End Value to compare iteration index with to signal end of iteration.
     * @tparam Increment Value to increment iteration index with.
     * @param r Reflector to operate on.
     * @param f Functor to invoke at each iteration. Needs to be able to take a string as the first parameter, and a generic type as the second parameter.
     *
     * Example:
     * @code{.cpp}
     *  for_i<0, refl.size, 1>(refl, [](std::string& name, auto& val) { print(name, val); });
     * @endcode
     */
    template<int I, int End, int Increment, typename Reflector, typename Func>
    void for_i(Reflector&& r, Func&& f)
    {
        detail::for_i_impl<I, std::less, End, Increment>(std::forward<Reflector>(r), std::forward<Func>(f));
    }
}

namespace std // NOLINT(cert-dcl58-cpp)
{
    template <::std::size_t I, typename T, typename ... Types>
    struct tuple_element<I, legion::core::reflector<T, Types...>>
    {
        using type = std::pair<std::string&, typename std::remove_reference_t<typename tuple_element<I, std::tuple<Types...>>::type>&>;
    };

    template <::std::size_t I, typename T, typename ... Types>
    tuple_element<I, legion::core::reflector<T, Types...>>
        get(legion::core::reflector<T, Types...>& val)
    {
        return std::make_pair(std::ref(val.names[I]), std::ref(std::get<I>(val.values)));
    }

    template <typename X, typename T, typename ... Types>
    tuple_element<legion::core::variable_index_v<X, legion::core::reflector<T, Types...>>,
        legion::core::reflector<T, Types...>>
        get(legion::core::reflector<T, Types...>& val)
    {
        return get<legion::core::variable_index_v<X, legion::core::reflector<T, Types...>>>(val);
    }

    template <typename T, typename ... Types>
    struct tuple_size<legion::core::reflector<T, Types...>>
        : public std::integral_constant<std::size_t, sizeof...(Types)>
    {
    };
    template <typename T, typename ... Types>
    struct tuple_size<const legion::core::reflector<T, Types...>>
        : public std::integral_constant<std::size_t, sizeof...(Types)>
    {
    };
    template <typename T, typename ... Types>
    struct tuple_size<volatile legion::core::reflector<T, Types...>>
        : public std::integral_constant<std::size_t, sizeof...(Types)>
    {
    };
    template <typename T, typename ... Types>
    struct tuple_size<const volatile legion::core::reflector<T, Types...>>
        : public std::integral_constant<std::size_t, sizeof...(Types)>
    {
    };

}
