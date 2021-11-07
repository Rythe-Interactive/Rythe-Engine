#pragma once
#include <cmath>

#include <core/types/primitives.hpp>
#include <core/math/vector/vector.hpp>

namespace legion::core::math
{
    namespace detail
    {
        template<typename _Scalar, size_type _Size>
        struct compute_pow
        {
            static constexpr size_type size = _Size;
            using value_type = vector<_Scalar, size>;

            L_ALWAYS_INLINE static value_type compute(const value_type& v, _Scalar s) noexcept
            {
                value_type result;
                for (size_type i; i < size; i++)
                    result[i] = ::std::pow(v[i], s);
                return result;
            }
        };

        template<typename _Scalar>
        struct compute_pow<_Scalar, 1>
        {
            static constexpr size_type size = 1;
            using value_type = vector<_Scalar, size>;

            L_ALWAYS_INLINE static value_type compute(const value_type& v, _Scalar s) noexcept
            {
                return ::std::pow(v.x, s);
            }
        };

        template<typename _Scalar>
        struct compute_pow<_Scalar, 2>
        {
            static constexpr size_type size = 2;
            using value_type = vector<_Scalar, size>;

            L_ALWAYS_INLINE static value_type compute(const value_type& v, _Scalar s) noexcept
            {
                return value_type{ ::std::pow(v.x, s), ::std::pow(v.y, s) };
            }
        };

        template<typename _Scalar>
        struct compute_pow<_Scalar, 3>
        {
            static constexpr size_type size = 3;
            using value_type = vector<_Scalar, size>;

            L_ALWAYS_INLINE static value_type compute(const value_type& v, _Scalar s) noexcept
            {
                return value_type{ ::std::pow(v.x, s), ::std::pow(v.y, s), ::std::pow(v.z, s) };
            }
        };

        template<typename _Scalar>
        struct compute_pow<_Scalar, 4>
        {
            static constexpr size_type size = 4;
            using value_type = vector<_Scalar, size>;

            L_ALWAYS_INLINE static value_type compute(const value_type& v, _Scalar s) noexcept
            {
                return value_type{ ::std::pow(v.x, s), ::std::pow(v.y, s), ::std::pow(v.z, s), ::std::pow(v.w, s) };
            }
        };
    }

    template<typename _Scalar, size_type _Size>
    L_ALWAYS_INLINE static vector<_Scalar, _Size> pow(const vector<_Scalar, _Size>& v) noexcept
    {
        return detail::compute_pow<_Scalar, _Size>::compute(v);
    }
}
