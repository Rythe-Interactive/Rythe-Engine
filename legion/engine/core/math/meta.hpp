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
}
