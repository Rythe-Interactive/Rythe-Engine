#pragma once
#include <core/types/primitives.hpp>
#include <core/math/vector/vector.hpp>

namespace legion::core::math
{
    namespace detail
    {
        template<typename Scalar, size_type Size>
        struct compute_cross
        {
            static constexpr size_type size = Size;
            using value_type = vector<Scalar, size>;

            constexpr static size_type index_at(size_type index)
            {
                return index % size;
            }

            constexpr static value_type compute(const value_type& a, const value_type& b) noexcept
            {
                value_type result;
                for (size_type i = 0; i < size; i++)
                    result[i] = a[index_at(i + 1)] * b[index_at(i + 2)] - b[index_at(i + 1)] * a[index_at(i + 2)];
                return result;
            }
        };
    }

    template<typename vec_type0, typename vec_type1, std::enable_if_t<is_vector_v<vec_type0>&& is_vector_v<vec_type1>, bool> = true>
    constexpr auto cross(const vec_type0& a, const vec_type1& b) noexcept
    {
        return detail::compute_cross<typename vec_type0::scalar, vec_type0::size>::compute(a, b);
    }
}
