#pragma once
#include <type_traits>

//god I want concepts

namespace legion::core::ecs::detail
{
    template<typename, typename T>
    struct has_init
    {
        static_assert(
            std::integral_constant<T, false>::value,
            "Second template param needs to be of function type.");
    };

    //requires signature type
    template <typename C, typename Ret, typename... Args>
    struct has_init<C, Ret(Args...)>
    {
    private:

        //specialization A, declval<T> has "to_resource" and signature matches
        template<typename T>
        static constexpr auto check(T*)
            -> typename std::is_same<decltype(std::declval<T>().init(std::declval<Args>()...)), Ret>::type;

        //specialization B, A failed
        template <typename>
        static constexpr auto check(...)
            ->std::false_type;

        //type of working specialization
        typedef decltype(check<C>(nullptr)) type;
    public:

        //value of type of specialization
        static constexpr bool value = type::value;
    };


    //check above
    template<typename, typename T>
    struct has_destroy
    {
        static_assert(
            std::integral_constant<T, false>::value,
            "Second template param needs to be of function type.");
    };

    template <typename C, typename Ret, typename... Args>
    struct has_destroy<C, Ret(Args...)>
    {
    private:
        template<typename T>
        static constexpr auto check(T*)
            -> typename std::is_same<decltype(std::declval<T>().destroy(std::declval<Args>()...)), Ret>::type;

        template <typename>
        static constexpr auto check(...)
            ->std::false_type;

        typedef decltype(check<C>(nullptr)) type;
    public:
        static constexpr bool value = type::value;
    };

}
