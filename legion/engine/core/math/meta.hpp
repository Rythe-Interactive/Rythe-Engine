#pragma once
#include <core/math/vector/vector_base.hpp>
#include <core/math/vector/swizzle/swizzle_base.hpp>

namespace legion::core::math
{
    template<typename T>
    struct is_vector
    {
        constexpr static bool value = false;
    };

    template<typename _Scalar, size_type _Size>
    struct is_vector<vector<_Scalar, _Size>>
    {
        constexpr static bool value = true;
    };

    template<typename _Scalar, size_type _Size, size_type... args>
    struct is_vector<detail::swizzle<_Scalar, _Size, args...>>
    {
        constexpr static bool value = true;
    };

    template<typename T>
    constexpr static bool is_vector_v = is_vector<T>::value;

    template<typename FPType>
    struct epsilon
    {
        static constexpr auto value = ::std::numeric_limits<FPType>::epsilon();
    };

    template<typename FPType>
    constexpr auto epsilon_v = epsilon<FPType>::value;

    namespace detail
    {
        template<typename TypeA, typename TypeB>
        constexpr auto highest_epsilon_impl()
        {
            if constexpr (epsilon_v<TypeA> > epsilon_v<TypeB>)
                return epsilon_v<TypeA>;
            else
                return epsilon_v<TypeB>;
        }
    }

    template<typename TypeA, typename TypeB>
    struct highest_epsilon
    {
        constexpr static auto value = detail::highest_epsilon_impl<TypeA, TypeB>();
    };

    template<typename TypeA, typename TypeB>
    constexpr auto highest_epsilon_v = highest_epsilon<TypeA, TypeB>::value;

    template<typename TypeA, typename TypeB>
    struct lowest_precision
    {
        using type = decltype(highest_epsilon_v<TypeA, TypeB>);
    };

    template<typename TypeA, typename TypeB>
    using lowest_precision_t = typename lowest_precision<TypeA, TypeB>::type;

    template<typename TypeA, typename TypeB, bool LHS = std::is_floating_point_v<TypeA>>
    struct floating_type;

    template<typename TypeA, typename TypeB>
    struct floating_type<TypeA, TypeB, true>
    {
        using type = TypeA;
    };

    template<typename TypeA, typename TypeB, bool LHS>
    struct floating_type
    {
        using type = TypeB;
    };

    template<typename TypeA, typename TypeB>
    using floating_type_t = typename floating_type<TypeA, TypeB>::type;
}
