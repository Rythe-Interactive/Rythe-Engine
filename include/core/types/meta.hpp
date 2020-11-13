#pragma once
#include <deque>
#include <vector>
#include <list>
#include <string>
#include <array>
#include <queue>
#include <type_traits>

namespace legion::core
{
    template<typename derived_type, typename base_type>
    using inherits_from = typename std::enable_if<std::is_base_of<base_type, derived_type>::value, int>::type;

    template<typename derived_type, typename base_type>
    using doesnt_inherit_from = typename std::enable_if<!std::is_base_of<base_type, derived_type>::value, int>::type;

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

    template <class T,size_t N>
    struct is_container<std::array<T,N>>
        :public std::true_type
    {};

    template <class T>
    struct is_container<std::queue<T>>
        :public std::true_type
    {};
}
