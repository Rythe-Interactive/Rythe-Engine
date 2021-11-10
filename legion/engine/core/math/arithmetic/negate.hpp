#pragma once
#include <core/types/primitives.hpp>
#include <core/math/vector/vector.hpp>

namespace legion::core::math
{
    namespace detail
    {
        template<typename _Scalar, size_type _Size>
        struct compute_negate
        {
            static constexpr size_type size = _Size;
            using value_type = vector<_Scalar, size>;

            constexpr static value_type compute(const value_type& a) noexcept
            {
                value_type result;
                for (size_type i = 0; i < size; i++)
                    result[i] = -a[i];
                return result;
            }

            constexpr static value_type& compute_assign(value_type& a) noexcept
            {
                for (size_type i = 0; i < size; i++)
                    a[i] = -a[i];
                return a;
            }
        };
    }

    template<typename _Scalar, size_type _Size>
    constexpr vector<_Scalar, _Size> negate(const vector<_Scalar, _Size>& a) noexcept
    {
        return detail::compute_negate<_Scalar, _Size>::compute(a);
    }

    template<typename _Scalar, size_t _Size>
    constexpr vector<_Scalar, _Size> operator-(const vector<_Scalar, _Size>& a)
    {
        return detail::compute_negate<_Scalar, _Size>::compute(a);
    }

    template<typename _Scalar, size_type _Size>
    constexpr vector<_Scalar, _Size>& negate_assign(vector<_Scalar, _Size>& a) noexcept
    {
        return detail::compute_negate<_Scalar, _Size>::compute_assign(a);
    }
}
