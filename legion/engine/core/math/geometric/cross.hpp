#pragma once
#include <core/types/primitives.hpp>
#include <core/math/vector/vector.hpp>

namespace legion::core::math
{
    namespace detail
    {
        template<typename _Scalar, size_type _Size>
        struct compute_cross
        {
            static constexpr size_type size = _Size;
            using value_type = vector<_Scalar, size>;

            constexpr static size_type index_at(size_type index)
            {
                return index % _Size;
            }

            constexpr static value_type compute(const value_type& a, const value_type& b) noexcept
            {
                value_type result;
                for (size_type i = 0; i < _Size; i++)
                    result[i] = a[index_at(i + 1)] * b[index_at(i + 2)] - b[index_at(i + 1)] * a[index_at(i + 2)];
                return result;
            }
        };
    }

    template<typename _Scalar, size_type _Size>
    constexpr vector<_Scalar, _Size> cross(const vector<_Scalar, _Size>& a, const vector<_Scalar, _Size>& b) noexcept
    {
        return detail::compute_cross<_Scalar, _Size>::compute(a, b);
    }
}
