#pragma once
#include <cmath>
#include <limits>
#include <core/types/primitives.hpp>
#include <core/math/meta.hpp>

namespace legion::core::math
{
    namespace detail
    {
        template<typename Scalar, size_type Size>
        struct compute_modf
        {
            static constexpr size_type size = Size;
            using value_type = vector<Scalar, size>;

            inline L_ALWAYS_INLINE static value_type compute(const value_type& val, value_type& integer) noexcept
            {
                    value_type result;
                    for (size_type i = 0; i < size; i++)
                        result[i] = ::std::modf(val[i], &integer[i]);
                    return result;
            }
        };

        template<typename Scalar>
        struct compute_modf<Scalar, 1u>
        {
            static constexpr size_type size = 1u;
            using value_type = vector<Scalar, size>;

            template<typename RET>
            inline L_ALWAYS_INLINE static value_type compute(const value_type& val, RET& integer) noexcept
            {
                if constexpr (is_vector_v<RET>)
                    return ::std::modf(val[0], &integer[0]);
                else
                    return ::std::modf(val[0], &integer);
            }
        };
    }

    template<typename T, typename RET>
    inline L_ALWAYS_INLINE static auto modf(const T& val, RET& integer)
    {
        if constexpr (is_vector_v<T>)
        {
            static_assert(::std::is_floating_point_v<typename T::scalar>, "Value must be floating point in order to use modf. (Did you mean fmod?)");
            return detail::compute_modf<typename T::scalar, T::size>::compute(val, integer);
        }
        else
        {
            static_assert(::std::is_floating_point_v<T>, "Value must be floating point in order to use modf. (Did you mean fmod?)");
            return ::std::modf(val, &integer);
        }
    }
}
