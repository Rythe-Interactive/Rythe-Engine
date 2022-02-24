#pragma once
#include <cmath>
#include <limits>

#include <core/platform/platform.hpp>
#include <core/types/primitives.hpp>
#include <core/math/meta.hpp>

namespace legion::core::math
{
    namespace detail
    {
        template<typename T>
        L_NODISCARD inline L_ALWAYS_INLINE static auto _abs_impl_(T val)
        {
            using value_type = remove_cvr_t<T>;
            if constexpr (::std::is_floating_point_v<value_type>)
            {
                value_type copy = val;
                byte* significantByte = reinterpret_cast<byte*>(&copy);

                if constexpr (endian::native == endian::little)
                    significantByte += sizeof(value_type) - 1;

                *significantByte &= static_cast<byte>(0b0111'1111);

                return copy;
            }
            else
                return val < 0 ? -val : val;
        }

        template<typename Scalar, size_type Size>
        struct compute_abs
        {
            static constexpr size_type size = Size;
            using value_type = vector<Scalar, size>;

            L_NODISCARD static value_type compute(const value_type& val) noexcept
            {
                if constexpr (!::std::is_signed_v<Scalar>)
                {
                    return val;
                }
                else
                {
                    value_type result;
                    for (size_type i = 0; i < size; i++)
                        result[i] = detail::_abs_impl_(val[i]);
                    return result;
                }
            }
        };

        template<typename Scalar>
        struct compute_abs<Scalar, 1u>
        {
            static constexpr size_type size = 1u;
            using value_type = vector<Scalar, size>;

            L_NODISCARD static Scalar compute(const value_type& val) noexcept
            {
                if constexpr (!::std::is_signed_v<Scalar>)
                {
                    return val;
                }
                else
                {
                    return detail::_abs_impl_(val[0u]);
                }
            }
        };
    }

    template<typename T>
    L_NODISCARD inline L_ALWAYS_INLINE static auto abs(T val)
    {
        using value_type = remove_cvr_t<T>;
        if constexpr (is_vector_v<value_type>)
        {
            return detail::compute_abs<typename value_type::scalar, value_type::size>::compute(val);
        }
        else if constexpr (!::std::is_signed_v<value_type>)
        {
            return val;
        }
        else
        {
            return detail::_abs_impl_(val);
        }        
    }
}
