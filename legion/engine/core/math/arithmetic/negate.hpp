#pragma once
#include <core/types/primitives.hpp>
#include <core/math/vector/vector.hpp>
#include <core/math/meta.hpp>

namespace legion::core::math
{
    namespace detail
    {
        template<typename _Scalar, size_type _Size>
        struct compute_negate
        {
            static constexpr size_type size = _Size;
            using value_type = vector<_Scalar, size>;

            L_NODISCARD constexpr static value_type compute(const value_type& a) noexcept
            {
                value_type result;
                for (size_type i = 0; i < size; i++)
                    result[i] = -a[i];
                return result;
            }
        };
    }

    template<typename vec_type, std::enable_if_t<is_vector_v<vec_type>, bool> = true>
    L_NODISCARD constexpr auto negate(const vec_type& a) noexcept
    {
        return detail::compute_negate<typename vec_type::scalar, vec_type::size>::compute(a);
    }

    template<typename vec_type, std::enable_if_t<is_vector_v<vec_type>, bool> = true>
    L_NODISCARD constexpr auto operator-(const vec_type& a)
    {
        return detail::compute_negate<typename vec_type::scalar, vec_type::size>::compute(a);
    }

    template<typename vec_type, std::enable_if_t<is_vector_v<vec_type>, bool> = true>
    constexpr vec_type& negate_assign(vec_type& a) noexcept
    {
        return a = detail::compute_negate<typename vec_type::scalar, vec_type::size>::compute(a);
    }
}
