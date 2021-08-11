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

    template<typename, typename T>
    struct has_resize
    {
        static_assert(
            std::integral_constant<T, false>::value,
            "Second template param needs to be of function type.");
    };

    template <typename C, typename Ret, typename... Args>
    struct has_resize<C, Ret(Args...)>
    {
    private:
        template<typename T>
        static constexpr auto check(T*)
            -> typename std::is_same<decltype(std::declval<T>().resize(std::declval<Args>()...)), Ret>::type;

        template <typename>
        static constexpr auto check(...)
            ->std::false_type;

        typedef decltype(check<C>(nullptr)) type;
    public:
        static constexpr bool value = type::value;
    };

    template <typename C, typename F>
    constexpr bool has_resize_v = has_resize<C, F>::value;

    template<typename T>
    struct has_size
    {
    private:
        typedef std::true_type yes;
        typedef std::false_type no;

        template<typename U> static auto test(int) -> decltype(std::declval<U>().size() == 1, yes());

        template<typename> static no test(...);

    public:

        static constexpr bool value = std::is_same<decltype(test<T>(0)), yes>::value;
    };

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

    template<typename T>
    struct is_pointer { static const bool value = false; };

    template<typename T>
    struct is_pointer<T*> { static const bool value = true; };


    template <class T>
    struct is_container
        : public std::false_type
    {};

    template <class T>
    struct is_container<std::vector<T>>
        : public std::true_type
    {};

    template <class T>
    struct is_container<std::basic_string<T>>
        :public std::true_type
    {};

    template <class T>
    struct is_container<std::deque<T>>
        :public std::true_type
    {};

    template <class T>
    struct is_container<std::list<T>>
        :public std::true_type
    {};

    template <class T, size_t N>
    struct is_container<std::array<T, N>>
        :public std::true_type
    {};

    template <class T>
    struct is_container<std::queue<T>>
        :public std::true_type
    {};
}
