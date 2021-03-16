#pragma once
#include <type_traits>

//god I want concepts

namespace legion::core::filesystem::detail
{
        template<typename,typename T>
        struct has_to_resource
        {
            static_assert(
                std::integral_constant<T,false>::value,
                "Second template param needs to be of function type.");
        };

        //requires signature type
        template <typename C,typename Ret,typename... Args>
        struct has_to_resource<C,Ret(Args...)>
        {
        private:

            //specialization A, declval<T> has "to_resource" and signature matches
            template<typename T>
            static constexpr auto check(T*)
                -> typename std::is_same<decltype(std::declval<T>().to_resource(std::declval<Args>()... )),Ret>::type;

            //specialization B, A failed
            template <typename>
            static constexpr auto check(...)
                -> std::false_type;

            //type of working specialization
            typedef decltype(check<C>(nullptr)) type;
        public:

            //value of type of specialization
            static constexpr bool value = type::value;
        };


        //check above
        template<typename,typename T>
        struct has_from_resource
        {
            static_assert(
                std::integral_constant<T,false>::value,
                "Second template param needs to be of function type.");
        };

        template <typename C,typename Ret,typename... Args>
        struct has_from_resource<C,Ret(Args...)>
        {
        private:
            template<typename T>
            static constexpr auto check(T*)
                -> typename std::is_same<decltype(std::declval<T>().from_resource(std::declval<Args>()... )),Ret>::type;

            template <typename>
            static constexpr auto check(...)
                -> std::false_type;

            typedef decltype(check<C>(nullptr)) type;
        public:
            static constexpr bool value = type::value;
        };
    
}
