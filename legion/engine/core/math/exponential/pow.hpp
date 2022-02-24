#pragma once
#include <cmath>

#include <core/types/primitives.hpp>
#include <core/math/vector/vector.hpp>

namespace legion::core::math
{
    namespace detail
    {
        template<typename Scalar, size_type Size>
        struct compute_pow
        {
            static constexpr size_type size = Size;
            using value_type = vector<Scalar, size>;

            L_ALWAYS_INLINE static value_type compute(const value_type& v, const value_type& s) noexcept
            {
                value_type result;
                for (size_type i; i < size; i++)
                    result[i] = ::std::pow(v[i], s[i]);
                return result;
            }

            L_ALWAYS_INLINE static value_type compute(const value_type& v, Scalar s) noexcept
            {
                value_type result;
                for (size_type i; i < size; i++)
                    result[i] = ::std::pow(v[i], s);
                return result;
            }
        };

        template<typename Scalar>
        struct compute_pow<Scalar, 1u>
        {
            static constexpr size_type size = 1u;
            using value_type = vector<Scalar, size>;

            L_ALWAYS_INLINE static value_type compute(const value_type& v, const value_type& s) noexcept
            {
                return ::std::pow(v[0], s[0]);
            }

            L_ALWAYS_INLINE static Scalar compute(const value_type& v, Scalar s) noexcept
            {
                return ::std::pow(v[0], s);
            }
        };

        template<typename Scalar>
        struct compute_pow<Scalar, 2u>
        {
            static constexpr size_type size = 2u;
            using value_type = vector<Scalar, size>;

            L_ALWAYS_INLINE static value_type compute(const value_type& v, const value_type& s) noexcept
            {
                return value_type{ ::std::pow(v[0], s[0]), ::std::pow(v[1], s[1]) };
            }

            L_ALWAYS_INLINE static value_type compute(const value_type& v, Scalar s) noexcept
            {
                return value_type{ ::std::pow(v[0], s), ::std::pow(v[1], s) };
            }
        };

        template<typename Scalar>
        struct compute_pow<Scalar, 3u>
        {
            static constexpr size_type size = 3u;
            using value_type = vector<Scalar, size>;

            L_ALWAYS_INLINE static value_type compute(const value_type& v, const value_type& s) noexcept
            {
                return value_type{ ::std::pow(v[0], s[0]), ::std::pow(v[1], s[1]), ::std::pow(v[2], s[2]) };
            }

            L_ALWAYS_INLINE static value_type compute(const value_type& v, Scalar s) noexcept
            {
                return value_type{ ::std::pow(v[0], s), ::std::pow(v[1], s), ::std::pow(v[2], s) };
            }
        };

        template<typename Scalar>
        struct compute_pow<Scalar, 4u>
        {
            static constexpr size_type size = 4u;
            using value_type = vector<Scalar, size>;

            L_ALWAYS_INLINE static value_type compute(const value_type& v, const value_type& s) noexcept
            {
                return value_type{ ::std::pow(v[0], s[0]), ::std::pow(v[1], s[1]), ::std::pow(v[2], s[2]), ::std::pow(v[3], s[3]) };
            }

            L_ALWAYS_INLINE static value_type compute(const value_type& v, Scalar s) noexcept
            {
                return value_type{ ::std::pow(v[0], s), ::std::pow(v[1], s), ::std::pow(v[2], s), ::std::pow(v[3], s) };
            }
        };
    }

    template<typename vec_type, ::std::enable_if_t<is_vector_v<vec_type>, bool> = true>
    L_ALWAYS_INLINE static auto pow(const vec_type& v, typename vec_type::scalar s) noexcept
    {
        return detail::compute_pow<typename vec_type::scalar, vec_type::size>::compute(v, s);
    }

    template<typename vec_type0, typename vec_type1, std::enable_if_t<is_vector_v<vec_type0>&& is_vector_v<vec_type1>, bool> = true>
    L_ALWAYS_INLINE static auto pow(const vec_type0& v, const vec_type1& s) noexcept
    {
        return detail::compute_pow<typename vec_type0::scalar, vec_type0::size>::compute(v, s);
    }

    template<typename Scalar, ::std::enable_if_t<!is_vector_v<remove_cvr_t<Scalar>>, bool> = true>
    L_ALWAYS_INLINE static auto pow(Scalar v, Scalar s) noexcept
    {
        return detail::compute_pow<Scalar, 1u>::compute(v, s);
    }
}
