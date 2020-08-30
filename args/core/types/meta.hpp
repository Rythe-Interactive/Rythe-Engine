#pragma once
#include <type_traits>

namespace args::core
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
}
