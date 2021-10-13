#pragma once

//#include "compute_common.hpp"
#include "setup.hpp"
#include <limits>

namespace legion::core::math{
namespace detail
{
    template <typename T, bool isFloat>
    struct compute_equal
    {
        GLM_FUNC_QUALIFIER GLM_CONSTEXPR static bool call(T a, T b)
        {
            return a == b;
        }
    };
}//namespace detail
}//namespace legion::core::math
