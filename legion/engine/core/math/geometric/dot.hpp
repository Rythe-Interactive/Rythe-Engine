#pragma once
#include <core/types/primitives.hpp>
#include <core/math/vector/vector.hpp>

namespace legion::core::math
{
    namespace detail
    {
        template<typename _Scalar, size_type _Size>
        struct compute_dot
        {
            static constexpr size_type size = _Size;
            using value_type = vector<_Scalar, size>;

            constexpr static _Scalar compute(const value_type& a, const value_type& b) noexcept
            {
                _Scalar result = 0;
                for (size_type i = 0; i < _Size; i++)
                    result += a[i] * b[i];
                return result;
            }
        };
    }

    template<typename _Scalar, size_type _Size>
    constexpr _Scalar dot(const vector<_Scalar, _Size>& a, const vector<_Scalar, _Size>& b) noexcept
    {
        return detail::compute_dot<_Scalar, _Size>::compute(a, b);
    }
}
