#pragma once
#include <cmath>

#include <core/types/primitives.hpp>
#include <core/math/vector/vector.hpp>
#include <core/math/geometric/dot.hpp>

namespace legion::core::math
{
    namespace detail
    {
        template<typename _Scalar, size_type _Size>
        struct compute_length2
        {
            static constexpr size_type size = _Size;
            using value_type = vector<_Scalar, size>;

            constexpr static _Scalar compute(const value_type& v) noexcept
            {
                return dot(v, v);
            }
        };
    }

    template<typename _Scalar, size_type _Size>
    constexpr _Scalar length2(const vector<_Scalar, _Size>& v) noexcept
    {
        return detail::compute_length2<_Scalar, _Size>::compute(v);
    }
}
