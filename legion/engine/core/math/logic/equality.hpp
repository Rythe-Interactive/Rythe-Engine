#pragma once
#include <core/types/primitives.hpp>
#include <core/math/vector/vector.hpp>
#include <core/math/util/close_enough.hpp>

namespace legion::core::math
{
    namespace detail
    {
        template<typename _Scalar, size_type _Size>
        struct compute_equality
        {
            static constexpr size_type size = _Size;
            using value_type = vector<_Scalar, size>;

            L_NODISCARD constexpr static vector<bool, _Size> compute(const value_type& a, const value_type& b) noexcept
            {
                vector<bool, size> result;
                for (size_type i = 0; i < size; i++)
                    result[i] = close_enough(a[i], b[i]);
                return result;
            }

            L_NODISCARD constexpr static vector<bool, _Size> compute(const value_type& a, _Scalar b) noexcept
            {
                vector<bool, size> result;
                for (size_type i = 0; i < size; i++)
                    result[i] = close_enough(a[i], b);
                return result;
            }
        };
    }

    template<typename vec_type0, typename vec_type1, std::enable_if_t<is_vector_v<vec_type0> && is_vector_v<vec_type1>, bool> = true>
    L_NODISCARD constexpr auto equals(const vec_type0& a, const vec_type1& b) noexcept
    {
        return detail::compute_equality<typename vec_type0::scalar, vec_type0::size>::compute(a, b);
    }

    template<typename vec_type0, typename vec_type1, std::enable_if_t<is_vector_v<vec_type0> && is_vector_v<vec_type1>, bool> = true>
    L_NODISCARD constexpr auto operator==(const vec_type0& a, const vec_type1& b) noexcept
    {
        return detail::compute_equality<typename vec_type0::scalar, vec_type0::size>::compute(a, b);
    }

    template<typename vec_type, std::enable_if_t<is_vector_v<vec_type>, bool> = true>
    L_NODISCARD constexpr auto equals(const vec_type& a, typename vec_type::scalar b) noexcept
    {
        return detail::compute_equality<typename vec_type::scalar, vec_type::size>::compute(a, b);
    }

    template<typename vec_type, std::enable_if_t<is_vector_v<vec_type>, bool> = true>
    L_NODISCARD constexpr auto operator==(const vec_type& a, typename vec_type::scalar b) noexcept
    {
        return detail::compute_equality<typename vec_type::scalar, vec_type::size>::compute(a, b);
    }
}
