#pragma once
#include <core/types/primitives.hpp>
#include <core/math/vector/vector.hpp>

namespace legion::core::math
{
    namespace detail
    {
        template<typename Scalar, size_type Size>
        struct compute_greater
        {
            static constexpr size_type size = Size;
            using value_type = vector<Scalar, size>;

            L_NODISCARD constexpr static vector<bool, size> compute(const value_type& a, const value_type& b) noexcept
            {
                vector<bool, size> result;
                for (size_type i = 0; i < size; i++)
                    result[i] = a[i] > b[i];
                return result;
            }

            L_NODISCARD constexpr static vector<bool, size> compute(const value_type& a, Scalar b) noexcept
            {
                vector<bool, size> result;
                for (size_type i = 0; i < size; i++)
                    result[i] = a[i] > b;
                return result;
            }
        };

        template<typename Scalar>
        struct compute_greater<Scalar, 1u>
        {
            static constexpr size_type size = 1u;
            using value_type = vector<Scalar, size>;

            L_NODISCARD constexpr static bool compute(const value_type& a, const value_type& b) noexcept
            {
                return a[0] > b[0];
            }

            L_NODISCARD constexpr static bool compute(const value_type& a, Scalar b) noexcept
            {
                return a[0] > b;
            }
        };
    }

    template<typename vec_type0, typename vec_type1, std::enable_if_t<is_vector_v<vec_type0>&& is_vector_v<vec_type1>, bool> = true>
    L_NODISCARD constexpr auto greater(const vec_type0& a, const vec_type1& b) noexcept
    {
        return detail::compute_greater<typename vec_type0::scalar, vec_type0::size>::compute(a, b);
    }

    template<typename vec_type0, typename vec_type1, std::enable_if_t<is_vector_v<vec_type0>&& is_vector_v<vec_type1>, bool> = true>
    L_NODISCARD constexpr auto operator>(const vec_type0& a, const vec_type1& b) noexcept
    {
        return detail::compute_greater<typename vec_type0::scalar, vec_type0::size>::compute(a, b);
    }

    template<typename vec_type, std::enable_if_t<is_vector_v<vec_type>, bool> = true>
    L_NODISCARD constexpr auto greater(const vec_type& a, typename vec_type::scalar b) noexcept
    {
        return detail::compute_greater<typename vec_type::scalar, vec_type::size>::compute(a, b);
    }

    template<typename vec_type, std::enable_if_t<is_vector_v<vec_type>, bool> = true>
    L_NODISCARD constexpr auto operator>(const vec_type& a, typename vec_type::scalar b) noexcept
    {
        return detail::compute_greater<typename vec_type::scalar, vec_type::size>::compute(a, b);
    }
}
