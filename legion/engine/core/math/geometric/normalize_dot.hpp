#pragma once
#include <core/math/geometric/dot.hpp>
#include <core/math/exponential/inverse_sqrt.hpp>

namespace legion::core::math
{
    template<typename _Scalar, size_type _Size>
    L_ALWAYS_INLINE static _Scalar normalize_dot(const vector<_Scalar, _Size>& a, const vector<_Scalar, _Size>& b) noexcept
    {
        return dot(a, b) * inv_sqrt(dot(a, a) * dot(b, b));
    }
}
