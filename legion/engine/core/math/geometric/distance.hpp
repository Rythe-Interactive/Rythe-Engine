#pragma once
#include <core/math/arithmetic/subtraction.hpp>
#include <core/math/geometric/length.hpp>

namespace legion::core::math
{
    template<typename _Scalar, size_type _Size>
    _Scalar dist(const vector<_Scalar, _Size>& a, const vector<_Scalar, _Size>& b) noexcept
    {
        return length(a - b);
    }

    template<typename _Scalar, size_type _Size>
    constexpr _Scalar dist2(const vector<_Scalar, _Size>& a, const vector<_Scalar, _Size>& b) noexcept
    {
        return length2(a - b);
    }
}
