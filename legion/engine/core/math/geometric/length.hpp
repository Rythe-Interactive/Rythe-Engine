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
        struct compute_length
        {
            static constexpr size_type size = _Size;
            using value_type = vector<_Scalar, size>;

            static _Scalar compute(const value_type& v) noexcept
            {
                return ::std::sqrt(dot(v, v));
            }
        };
    }

    template<typename _Scalar, size_type _Size>
    _Scalar length(const vector<_Scalar, _Size>& v) noexcept
    {
        return detail::compute_length<_Scalar, _Size>::compute(v);
    }
}
