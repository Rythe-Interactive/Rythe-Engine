#pragma once
#include <core/types/primitives.hpp>
#include <core/math/vector/vector.hpp>

namespace legion::core::math
{
    namespace detail
    {
        template<typename _Scalar, size_type _Size>
        struct compute_subtraction
        {
            static constexpr size_type size = _Size;
            using value_type = vector<_Scalar, size>;

            constexpr static value_type compute(const value_type& a, const value_type& b) noexcept
            {
                value_type result;
                for (size_type i = 0; i < size; i++)
                    result[i] = a[i] - b[i];
                return result;
            }

            constexpr static value_type compute(const value_type& a, _Scalar b) noexcept
            {
                value_type result;
                for (size_type i = 0; i < size; i++)
                    result[i] = a[i] - b;
                return result;
            }

            constexpr static value_type& compute_assign(value_type& a, const value_type& b) noexcept
            {
                for (size_type i = 0; i < size; i++)
                    a[i] -= b[i];
                return a;
            }

            constexpr static value_type& compute_assign(value_type& a, _Scalar b) noexcept
            {
                value_type result;
                for (size_type i = 0; i < size; i++)
                    a[i] -= b;
                return a;
            }
        };
    }

    template<typename _Scalar, size_type _Size>
    constexpr vector<_Scalar, _Size> sub(const vector<_Scalar, _Size>& a, const vector<_Scalar, _Size>& b) noexcept
    {
        return detail::compute_subtraction<_Scalar, _Size>::compute(a, b);
    }

    template<typename _Scalar, size_t _Size>
    constexpr vector<_Scalar, _Size> operator-(const vector<_Scalar, _Size>& a, const vector<_Scalar, _Size>& b)
    {
        return detail::compute_subtraction<_Scalar, _Size>::compute(a, b);
    }

    template<typename _Scalar, size_type _Size>
    constexpr vector<_Scalar, _Size> sub(const vector<_Scalar, _Size>& a, _Scalar b) noexcept
    {
        return detail::compute_subtraction<_Scalar, _Size>::compute(a, b);
    }

    template<typename _Scalar, size_t _Size>
    constexpr vector<_Scalar, _Size> operator-(const vector<_Scalar, _Size>& a, _Scalar b)
    {
        return detail::compute_subtraction<_Scalar, _Size>::compute(a, b);
    }

    template<typename _Scalar, size_type _Size>
    constexpr vector<_Scalar, _Size>& sub_assign(vector<_Scalar, _Size>& a, const vector<_Scalar, _Size>& b) noexcept
    {
        return detail::compute_subtraction<_Scalar, _Size>::compute_assign(a, b);
    }

    template<typename _Scalar, size_t _Size>
    constexpr vector<_Scalar, _Size>& operator-=(vector<_Scalar, _Size>& a, const vector<_Scalar, _Size>& b)
    {
        return detail::compute_subtraction<_Scalar, _Size>::compute_assign(a, b);
    }

    template<typename _Scalar, size_type _Size>
    constexpr vector<_Scalar, _Size>& sub_assign(vector<_Scalar, _Size>& a, _Scalar b) noexcept
    {
        return detail::compute_subtraction<_Scalar, _Size>::compute_assign(a, b);
    }

    template<typename _Scalar, size_t _Size>
    constexpr vector<_Scalar, _Size>& operator-=(vector<_Scalar, _Size>& a, _Scalar b)
    {
        return detail::compute_subtraction<_Scalar, _Size>::compute_assign(a, b);
    }
}
