#pragma once
#include <cmath>
#include <limits>
#include <core/types/primitives.hpp>
#include <core/math/meta.hpp>

namespace legion::core::math
{
    namespace detail
    {
        template<typename _Scalar, size_type _Size>
        struct compute_abs;
    }

    template<typename T>
    constexpr static T abs(T val)
    {
        if constexpr (!std::numeric_limits<T>::is_signed)
        {
            return val;
        }
        else if constexpr (std::is_arithmetic_v<T>)
        {
            return val >= static_cast<T>(0) ? val : -val;
        }
        else if constexpr (is_vector_v<T>)
        {
            return detail::compute_abs<typename T::scalar, T::size>::compute(val);
        }
        else
        {
            return ::std::abs(val);
        }
    }

    namespace detail
    {
        template<typename _Scalar, size_type _Size>
        struct compute_abs
        {
            static constexpr size_type size = _Size;
            using value_type = vector<_Scalar, size>;

            constexpr static value_type compute(const value_type& val) noexcept
            {
                if constexpr (!std::numeric_limits<_Scalar>::is_signed)
                {
                    return val;
                }
                else
                {
                    value_type result;
                    for (size_type i = 0; i < size; i++)
                        result[i] = abs(val[i]);
                    return result;
                }
            }
        };
    }
}
