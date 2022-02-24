#pragma once
#include <core/types/primitives.hpp>
#include <core/math/vector/vector.hpp>

namespace legion::core::math
{
    namespace detail
    {
        template<typename Scalar, size_type Size>
        struct compute_dot
        {
            static constexpr size_type size = Size;
            using value_type = vector<Scalar, size>;

            constexpr static Scalar compute(const value_type& a, const value_type& b) noexcept
            {
                Scalar result = 0;
                for (size_type i = 0; i < size; i++)
                    result += a[i] * b[i];
                return result;
            }
        };
    }

    template<typename vec_type0, typename vec_type1, std::enable_if_t<is_vector_v<vec_type0>&& is_vector_v<vec_type1>, bool> = true>
    constexpr auto dot(const vec_type0& a, const vec_type1& b) noexcept
    {
        return detail::compute_dot<typename vec_type0::scalar, vec_type0::size>::compute(a, b);
    }
}
