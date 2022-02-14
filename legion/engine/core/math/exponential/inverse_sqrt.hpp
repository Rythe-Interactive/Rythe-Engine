#pragma once
#include <cmath>

#include <core/types/primitives.hpp>
#include <core/math/vector/vector.hpp>

namespace legion::core::math
{
    namespace detail
    {
        template<typename _Scalar, size_type _Size>
        struct compute_inv_sqrt
        {
            static constexpr size_type size = _Size;
            using value_type = vector<_Scalar, size>;

            L_ALWAYS_INLINE static value_type compute(const value_type& v) noexcept
            {
                value_type result;
                for (size_type i; i < size; i++)
                    result[i] = static_cast<_Scalar>(1) / ::std::sqrt(v[i]);
                return result;
            }
        };

        template<typename _Scalar>
        struct compute_inv_sqrt<_Scalar, 1>
        {
            static constexpr size_type size = 1;
            using value_type = vector<_Scalar, size>;

            L_ALWAYS_INLINE static value_type compute(const value_type& v) noexcept
            {
                return static_cast<_Scalar>(1) / ::std::sqrt(v.x);
            }
        };

        template<typename _Scalar>
        struct compute_inv_sqrt<_Scalar, 2>
        {
            static constexpr size_type size = 2;
            using value_type = vector<_Scalar, size>;

            L_ALWAYS_INLINE static value_type compute(const value_type& v) noexcept
            {
                return value_type{ static_cast<_Scalar>(1) / ::std::sqrt(v.x), static_cast<_Scalar>(1) / ::std::sqrt(v.y) };
            }
        };

        template<typename _Scalar>
        struct compute_inv_sqrt<_Scalar, 3>
        {
            static constexpr size_type size = 3;
            using value_type = vector<_Scalar, size>;

            L_ALWAYS_INLINE static value_type compute(const value_type& v) noexcept
            {
                return value_type{
                    static_cast<_Scalar>(1) / ::std::sqrt(v.x),
                    static_cast<_Scalar>(1) / ::std::sqrt(v.y),
                    static_cast<_Scalar>(1) / ::std::sqrt(v.z) };
            }
        };

        template<typename _Scalar>
        struct compute_inv_sqrt<_Scalar, 4>
        {
            static constexpr size_type size = 4;
            using value_type = vector<_Scalar, size>;

            L_ALWAYS_INLINE static value_type compute(const value_type& v) noexcept
            {
                return value_type{
                    static_cast<_Scalar>(1) / ::std::sqrt(v.x),
                    static_cast<_Scalar>(1) / ::std::sqrt(v.y),
                    static_cast<_Scalar>(1) / ::std::sqrt(v.z),
                    static_cast<_Scalar>(1) / ::std::sqrt(v.w) };
            }
        };
    }

    template<typename _Scalar, size_type _Size>
    L_ALWAYS_INLINE static vector<_Scalar, _Size> inv_sqrt(const vector<_Scalar, _Size>& v) noexcept
    {
        return detail::compute_inv_sqrt<_Scalar, _Size>::compute(v);
    }

    template<typename _Scalar, size_type _Size>
    L_ALWAYS_INLINE static vector<_Scalar, _Size> inverse_sqrt(const vector<_Scalar, _Size>& v) noexcept
    {
        return detail::compute_inv_sqrt<_Scalar, _Size>::compute(v);
    }

    template<typename _Scalar>
    L_ALWAYS_INLINE static vector<_Scalar, 1> inv_sqrt(_Scalar v) noexcept
    {
        return detail::compute_inv_sqrt<_Scalar, 1>::compute(v);
    }

    template<typename _Scalar>
    L_ALWAYS_INLINE static vector<_Scalar, 1> inverse_sqrt(_Scalar v) noexcept
    {
        return detail::compute_inv_sqrt<_Scalar, 1>::compute(v);
    }
}
