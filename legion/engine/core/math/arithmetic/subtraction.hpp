#pragma once
#include <core/types/primitives.hpp>
#include <core/math/vector/vector.hpp>
#include <core/math/meta.hpp>

namespace legion::core::math
{
    namespace detail
    {
        template<typename Scalar, size_type Size>
        struct compute_subtraction
        {
            static constexpr size_type size = Size;
            using value_type = vector<Scalar, size>;

            L_NODISCARD constexpr static value_type compute(const value_type& a, const value_type& b) noexcept
            {
                value_type result;
                for (size_type i = 0; i < size; i++)
                    result[i] = a[i] - b[i];
                return result;
            }

            L_NODISCARD constexpr static value_type compute(const value_type& a, Scalar b) noexcept
            {
                value_type result;
                for (size_type i = 0; i < size; i++)
                    result[i] = a[i] - b;
                return result;
            }
        };

        template<typename Scalar>
        struct compute_subtraction<Scalar, 1u>
        {
            static constexpr size_type size = 1u;
            using value_type = vector<Scalar, size>;

            L_NODISCARD constexpr static Scalar compute(const value_type& a, const value_type& b) noexcept
            {
                return a[0] - b[0];
            }

            L_NODISCARD constexpr static Scalar compute(const value_type& a, Scalar b) noexcept
            {
                return a[0] - b;
            }
        };
    }

    template<typename vec_type0, typename vec_type1, std::enable_if_t<is_vector_v<vec_type0>&& is_vector_v<vec_type1>, bool> = true>
    L_NODISCARD constexpr auto sub(const vec_type0& a, const vec_type1& b) noexcept
    {
        return detail::compute_subtraction<typename vec_type0::scalar, vec_type0::size>::compute(a, b);
    }

    template<typename vec_type0, typename vec_type1, std::enable_if_t<is_vector_v<vec_type0>&& is_vector_v<vec_type1>, bool> = true>
    L_NODISCARD constexpr auto operator-(const vec_type0& a, const vec_type1& b)
    {
        return detail::compute_subtraction<typename vec_type0::scalar, vec_type0::size>::compute(a, b);
    }

    template<typename vec_type, std::enable_if_t<is_vector_v<vec_type>, bool> = true>
    L_NODISCARD constexpr auto sub(const vec_type& a, typename vec_type::scalar b) noexcept
    {
        return detail::compute_subtraction<typename vec_type::scalar, vec_type::size>::compute(a, b);
    }

    template<typename vec_type, std::enable_if_t<is_vector_v<vec_type>, bool> = true>
    L_NODISCARD constexpr auto operator-(const vec_type& a, typename vec_type::scalar b)
    {
        return detail::compute_subtraction<typename vec_type::scalar, vec_type::size>::compute(a, b);
    }

    template<typename vec_type0, typename vec_type1, std::enable_if_t<is_vector_v<vec_type0>&& is_vector_v<vec_type1>, bool> = true>
    constexpr vec_type0& sub_assign(vec_type0& a, const vec_type1& b) noexcept
    {
        return a = detail::compute_subtraction<typename vec_type0::scalar, vec_type0::size>::compute(a, b);
    }

    template<typename vec_type0, typename vec_type1, std::enable_if_t<is_vector_v<vec_type0>&& is_vector_v<vec_type1>, bool> = true>
    constexpr vec_type0& operator-=(vec_type0& a, const vec_type1& b)
    {
        return a = detail::compute_subtraction<typename vec_type0::scalar, vec_type0::size>::compute(a, b);
    }

    template<typename vec_type, std::enable_if_t<is_vector_v<vec_type>, bool> = true>
    constexpr vec_type& sub_assign(vec_type& a, typename vec_type::scalar b) noexcept
    {
        return a = detail::compute_subtraction<typename vec_type::scalar, vec_type::size>::compute(a, b);
    }

    template<typename vec_type, std::enable_if_t<is_vector_v<vec_type>, bool> = true>
    constexpr vec_type& operator-=(vec_type& a, typename vec_type::scalar b)
    {
        return a = detail::compute_subtraction<typename vec_type::scalar, vec_type::size>::compute(a, b);
    }
}
