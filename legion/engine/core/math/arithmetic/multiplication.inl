#include <core/math/arithmetic/multiplication.hpp>
#pragma once

namespace legion::core::math
{
    namespace detail
    {
        template<typename Scalar, size_type Size>
        L_NODISCARD constexpr auto compute_multiplication<vector<Scalar, Size>>::compute(const value_type& a, const value_type& b) noexcept
        {
            value_type result;
            for (size_type i = 0; i < Size; i++)
                result[i] = a[i] * b[i];
            return result;
        }

        template<typename Scalar, size_type Size>
        L_NODISCARD constexpr auto compute_multiplication<vector<Scalar, Size>>::compute(const value_type& a, Scalar b) noexcept
        {
            value_type result;
            for (size_type i = 0; i < Size; i++)
                result[i] = a[i] * b;
            return result;
        }


        template<typename Scalar>
        L_NODISCARD constexpr Scalar compute_multiplication<vector<Scalar, 1u>>::compute(const value_type& a, Scalar b) noexcept
        {
            return a[0] * b;
        }
    }

    template<typename vec_type0, typename vec_type1, std::enable_if_t<is_vector_v<vec_type0>&& is_vector_v<vec_type1>, bool>>
    L_NODISCARD constexpr auto mul(const vec_type0& a, const vec_type1& b) noexcept
    {
        return detail::compute_multiplication<typename vec_type0::scalar, vec_type0::size>::compute(a, b);
    }

    template<typename vec_type0, typename vec_type1, std::enable_if_t<is_vector_v<vec_type0>&& is_vector_v<vec_type1>, bool>>
    L_NODISCARD constexpr auto operator*(const vec_type0& a, const vec_type1& b)
    {
        return detail::compute_multiplication<typename vec_type0::scalar, vec_type0::size>::compute(a, b);
    }

    template<typename vec_type, std::enable_if_t<is_vector_v<vec_type>, bool> = true>
    L_NODISCARD constexpr auto mul(const vec_type& a, typename vec_type::scalar b) noexcept
    {
        return detail::compute_multiplication<typename vec_type::scalar, vec_type::size>::compute(a, b);
    }

    template<typename vec_type, std::enable_if_t<is_vector_v<vec_type>, bool> = true>
    L_NODISCARD constexpr auto operator*(const vec_type& a, typename vec_type::scalar b)
    {
        return detail::compute_multiplication<typename vec_type::scalar, vec_type::size>::compute(a, b);
    }

    template<typename vec_type0, typename vec_type1, std::enable_if_t<is_vector_v<vec_type0>&& is_vector_v<vec_type1>, bool>>
    constexpr vec_type0& mul_assign(vec_type0& a, const vec_type1& b) noexcept
    {
        return a = detail::compute_multiplication<typename vec_type0::scalar, vec_type0::size>::compute(a, b);
    }

    template<typename vec_type0, typename vec_type1, std::enable_if_t<is_vector_v<vec_type0>&& is_vector_v<vec_type1>, bool>>
    constexpr vec_type0& operator*=(vec_type0& a, const vec_type1& b)
    {
        return a = detail::compute_multiplication<typename vec_type0::scalar, vec_type0::size>::compute(a, b);
    }

    template<typename vec_type, std::enable_if_t<is_vector_v<vec_type>, bool>>
    constexpr vec_type& mul_assign(vec_type& a, typename vec_type::scalar b) noexcept
    {
        return a = detail::compute_multiplication<typename vec_type::scalar, vec_type::size>::compute(a, b);
    }

    template<typename vec_type, std::enable_if_t<is_vector_v<vec_type>, bool>>
    constexpr vec_type& operator*=(vec_type& a, typename vec_type::scalar b)
    {
        return a = detail::compute_multiplication<typename vec_type::scalar, vec_type::size>::compute(a, b);
    }
}
