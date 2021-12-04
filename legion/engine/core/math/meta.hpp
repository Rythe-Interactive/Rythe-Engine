#pragma once
#include <core/math/vector/vector.hpp>

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

    template<typename T>
    constexpr static bool is_vector_v = is_vector<T>::value;
}
