#pragma once
#include <type_traits>

namespace legion::core::serialization
{
    //template<typename, typename T>
    //struct has_serialize
    //{
    //    static_assert(
    //        std::integral_constant<T, false>::value,
    //        "Second template param needs to be of function type.");
    //};

    //template <typename C, typename Ret, typename... Args>
    //struct has_serialize<C, Ret(Args...)>
    //{
    //private:
    //    template<typename T>
    //    static constexpr auto check(T*)
    //        -> typename std::is_same<decltype(std::declval<T>().serialize(std::declval<Args>()...)), Ret>::type;

    //    template <typename>
    //    static constexpr auto check(...)
    //        ->std::false_type;

    //    typedef decltype(check<C>(nullptr)) type;
    //public:
    //    static constexpr bool value = type::value;
    //};


    //template<typename, typename T>
    //struct has_save
    //{
    //    static_assert(
    //        std::integral_constant<T, false>::value,
    //        "Second template param needs to be of function type.");
    //};

    //template <typename C, typename Ret, typename... Args>
    //struct has_save<C, Ret(Args...)>
    //{
    //private:
    //    template<typename T>
    //    static constexpr auto check(T*)
    //        -> typename std::is_same<decltype(std::declval<T>().save(std::declval<Args>()...)), Ret>::type;

    //    template <typename>
    //    static constexpr auto check(...)
    //        ->std::false_type;

    //    typedef decltype(check<C>(nullptr)) type;
    //public:
    //    static constexpr bool value = type::value;
    //};

    //template<typename, typename T>
    //struct has_load
    //{
    //    static_assert(
    //        std::integral_constant<T, false>::value,
    //        "Second template param needs to be of function type.");
    //};

    //template <typename C, typename Ret, typename... Args>
    //struct has_load<C, Ret(Args...)>
    //{
    //private:
    //    template<typename T>
    //    static constexpr auto check(T*)
    //        -> typename std::is_same< decltype(std::declval<T>().load(std::declval<Args>()...)), Ret>::type;

    //    template <typename>
    //    static constexpr auto check(...)
    //        ->std::false_type;

    //    typedef decltype(check<C>(nullptr)) type;
    //public:
    //    static constexpr bool value = type::value;
    //};

    //template<typename, typename T>
    //struct is_container
    //{
    //    static_assert(
    //        std::integral_constant<T, false>::value,
    //        "Second template param needs to be of function type.");
    //};

    //template <typename C, typename Ret>
    //struct is_container<C,Ret()>
    //{
    //private:
    //    template<typename T>
    //    static constexpr auto check(T*)
    //        -> typename std::is_same<decltype(std::declval<T>().size()), Ret>::type;

    //    template <typename>
    //    static constexpr auto check(...)
    //        ->std::false_type;

    //    typedef decltype(check<C>(nullptr)) type;
    //public:
    //    static constexpr bool value = type::value;
    //};

}
