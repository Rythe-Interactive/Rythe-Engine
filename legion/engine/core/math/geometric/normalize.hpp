#pragma once
#include <cmath>

#include <core/types/primitives.hpp>
#include <core/math/vector/vector.hpp>
#include <core/math/arithmetic/division.hpp>
#include <core/math/geometric/length.hpp>

namespace legion::core::math
{
    namespace detail
    {
        template<typename _Scalar, size_type _Size>
        struct compute_normalize
        {
            static constexpr size_type size = _Size;
            using value_type = vector<_Scalar, size>;

            L_ALWAYS_INLINE static value_type compute(const value_type& v) noexcept
            {
                return v / length(v);
            }
        };
    }

    template<typename _Scalar, size_type _Size>
    constexpr auto normalize(const vector<_Scalar, _Size>& v) noexcept
    {
        return detail::compute_normalize<_Scalar, _Size>::compute(v);
    }
}
