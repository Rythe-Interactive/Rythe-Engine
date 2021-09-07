#pragma once
#include <deque>
#include <vector>
#include <list>
#include <string>
#include <array>
#include <queue>
#include <type_traits>

#include <core/platform/platform.hpp>
#include <core/types/primitives.hpp>

namespace legion::core
{

#define HAS_FUNC(x)                                                                                                     \
    template<typename, typename T>                                                                                      \
    struct CONCAT(has_, x) {                                                                                            \
        static_assert(std::integral_constant<T, false>::value, "Second template param needs to be of function type.");  \
    };                                                                                                                  \
                                                                                                                        \
    template<typename C, typename Ret, typename... Args>                                                                \
    struct CONCAT(has_, x)<C, Ret(Args...)> {                                                                           \
    private:                                                                                                            \
        template<typename T>                                                                                            \
        static constexpr auto check(T*)                                                                                 \
            -> typename std::is_same<decltype(std::declval<T>(). x (std::declval<Args>()...)), Ret>::type;              \
                                                                                                                        \
        template <typename>                                                                                             \
        static constexpr auto check(...)                                                                                \
            ->std::false_type;                                                                                          \
                                                                                                                        \
        typedef decltype(check<C>(nullptr)) type;                                                                       \
    public:                                                                                                             \
        static constexpr bool value = type::value;                                                                      \
    };                                                                                                                  \
                                                                                                                        \
    template<typename C, typename F>                                                                                    \
    constexpr bool CONCAT_DEFINE(has_, CONCAT(x, _v)) = CONCAT(has_, x)<C, F>::value;                                   \
                                                                                                                        \
    template<typename, typename T>                                                                                      \
    struct CONCAT(has_static_, x) {                                                                                     \
        static_assert(std::integral_constant<T, false>::value, "Second template param needs to be of function type.");  \
    };                                                                                                                  \
                                                                                                                        \
    template<typename C, typename Ret, typename... Args>                                                                \
    struct CONCAT(has_static_, x)<C, Ret(Args...)> {                                                                    \
    private:                                                                                                            \
        template<typename T>                                                                                            \
        static constexpr auto check(T*)                                                                                 \
            -> typename std::is_same<decltype(T:: x (std::declval<Args>()...)), Ret>::type;                             \
                                                                                                                        \
        template <typename>                                                                                             \
        static constexpr auto check(...)                                                                                \
            ->std::false_type;                                                                                          \
                                                                                                                        \
        typedef decltype(check<C>(nullptr)) type;                                                                       \
    public:                                                                                                             \
        static constexpr bool value = type::value;                                                                      \
    };                                                                                                                  \
                                                                                                                        \
    template<typename C, typename F>                                                                                    \
    constexpr bool CONCAT_DEFINE(has_static_, CONCAT(x, _v)) = CONCAT(has_static_, x)<C, F>::value;

#define  typename_1(x)                                                                    typename x
#define  typename_2(x, x2)                                                                typename x , typename x2
#define  typename_3(x, x2, x3)                                                            typename x , typename x2, typename x3
#define  typename_4(x, x2, x3, x4)                                                        typename x , typename x2, typename x3, typename x4
#define  typename_5(x, x2, x3, x4, x5)                                                    typename x , typename x2, typename x3, typename x4, typename x5
#define  typename_6(x, x2, x3, x4, x5, x6)                                                typename x , typename x2, typename x3, typename x4, typename x5, typename x6
#define  typename_7(x, x2, x3, x4, x5, x6, x7)                                            typename x , typename x2, typename x3, typename x4, typename x5, typename x6, typename x7
#define  typename_8(x, x2, x3, x4, x5, x6, x7, x8)                                        typename x , typename x2, typename x3, typename x4, typename x5, typename x6, typename x7, typename x8
#define  typename_9(x, x2, x3, x4, x5, x6, x7, x8, x9)                                    typename x , typename x2, typename x3, typename x4, typename x5, typename x6, typename x7, typename x8, typename x9
#define typename_10(x, x2, x3, x4, x5, x6, x7, x8, x9, x10)                               typename x , typename x2, typename x3, typename x4, typename x5, typename x6, typename x7, typename x8, typename x9, typename x10
#define typename_11(x, x2, x3, x4, x5, x6, x7, x8, x9, x10, x11)                          typename x , typename x2, typename x3, typename x4, typename x5, typename x6, typename x7, typename x8, typename x9, typename x10, typename x11
#define typename_12(x, x2, x3, x4, x5, x6, x7, x8, x9, x10, x11, x12)                     typename x , typename x2, typename x3, typename x4, typename x5, typename x6, typename x7, typename x8, typename x9, typename x10, typename x11, typename x12
#define typename_13(x, x2, x3, x4, x5, x6, x7, x8, x9, x10, x11, x12, x13)                typename x , typename x2, typename x3, typename x4, typename x5, typename x6, typename x7, typename x8, typename x9, typename x10, typename x11, typename x12, typename x13
#define typename_14(x, x2, x3, x4, x5, x6, x7, x8, x9, x10, x11, x12, x13, x14)           typename x , typename x2, typename x3, typename x4, typename x5, typename x6, typename x7, typename x8, typename x9, typename x10, typename x11, typename x12, typename x13, typename x14
#define typename_15(x, x2, x3, x4, x5, x6, x7, x8, x9, x10, x11, x12, x13, x14, x15)      typename x , typename x2, typename x3, typename x4, typename x5, typename x6, typename x7, typename x8, typename x9, typename x10, typename x11, typename x12, typename x13, typename x14, typename x15
#define typename_16(x, x2, x3, x4, x5, x6, x7, x8, x9, x10, x11, x12, x13, x14, x15, x16) typename x , typename x2, typename x3, typename x4, typename x5, typename x6, typename x7, typename x8, typename x9, typename x10, typename x11, typename x12, typename x13, typename x14, typename x15, typename x16

    // turn: a, b, ...c
    // into: typename a, typename b, typename ...c
#define typenames_count(count, ...) EXPAND(CONCAT_DEFINE(typename_, count)(__VA_ARGS__))
#define typenames(...) EXPAND(CONCAT_DEFINE(typename_, NARGS(__VA_ARGS__))(__VA_ARGS__))

#define COMBINE_SFINAE(name, predicate, templateArgs...)                                                                \
    template<typenames(templateArgs)>                                                                                   \
    struct name                                                                                                         \
    {                                                                                                                   \
        static constexpr bool value = predicate;                                                                        \
    };                                                                                                                  \
                                                                                                                        \
    template<EXPAND(typenames(EXPAND(templateArgs)))>                                                                   \
    constexpr bool CONCAT(name, _v) = name<EXPAND(templateArgs)>::value;

    HAS_FUNC(begin);
    HAS_FUNC(end);

    COMBINE_SFINAE(is_container, has_begin_v<T COMMA typename T::iterator(void)> && has_end_v<T COMMA typename T::iterator(void)>, T);

    HAS_FUNC(resize);

    COMBINE_SFINAE(is_resizable_container, has_begin_v<T COMMA typename T::iterator(void)> && has_end_v<T COMMA typename T::iterator(void)> && has_resize_v<T COMMA void(size_type)>, T);

    HAS_FUNC(setup);
    HAS_FUNC(update);



    template<typename derived_type, typename base_type>
    using inherits_from = typename std::enable_if<std::is_base_of<base_type, derived_type>::value, int>::type;

    template<typename derived_type, typename base_type>
    using doesnt_inherit_from = typename std::enable_if<!std::is_base_of<base_type, derived_type>::value, int>::type;

    template<typename T>
    using remove_cvr = std::remove_cv<std::remove_reference_t<T>>;

    template<typename T>
    using remove_cvr_t = typename remove_cvr<T>::type;

    template <class T>
    struct is_vector
        : public std::false_type
    {
    };

    template <class T>
    struct is_vector<std::vector<T>>
        : public std::true_type
    {
    };

    template<template<typename...>typename T, typename U, size_type I, typename... Args>
    struct make_sequence : make_sequence<T, U, I - 1, Args..., U> {};

    template<template<typename...>typename T, typename U, typename... Args>
    struct make_sequence<T, U, 0, Args...>
    {
        using type = T<Args...>;
    };

    template<template<typename...>typename T, typename U, size_type I, typename... Args>
    using make_sequence_t = typename make_sequence<T, U, I, Args...>::type;

    template<template<typename>typename Compare, typename T, T A, T B>
    struct do_compare
    {
        static constexpr inline Compare<T> comp{};
        static constexpr inline bool value = comp(A, B);
    };

    template<template<typename>typename Compare, typename T, T A, T B>
    inline constexpr bool do_compare_v = do_compare<Compare, T, A, B>::value;

    template<size_type I, typename Type, typename... Types>
    struct element_at : element_at<I - 1, Types...>
    {
    };

    template<typename Type, typename... Types>
    struct element_at<0, Type, Types...>
    {
        using type = Type;
    };

    template<size_type I, typename Type, typename... Types>
    using element_at_t = typename element_at<I, Type, Types...>::type;

    template<class T, typename... Args>
    decltype(void(T{ std::declval<Args>()... }), std::true_type())
        brace_construct_test(int);

    template<class T, typename... Args>
    std::false_type
        brace_construct_test(...);

    template<class T, typename... Args>
    struct is_brace_constructible : decltype(brace_construct_test<T, Args...>(0))
    {
    };

    template<class T, typename... Args>
    inline constexpr bool is_brace_constructible_v = is_brace_constructible<T, Args...>::value;
}
