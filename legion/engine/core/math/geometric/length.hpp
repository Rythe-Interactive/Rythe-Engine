#pragma once
#include <cmath>

#include <core/types/primitives.hpp>
#include <core/math/vector/vector.hpp>
#include <core/math/geometric/dot.hpp>

namespace legion::core::math
{
    namespace detail
    {
        template<typename Scalar, size_type Size>
        struct compute_length
        {
            static constexpr size_type size = Size;
            using value_type = vector<Scalar, size>;

            static Scalar compute(const value_type& v) noexcept
            {
                return ::std::sqrt(dot(v, v));
            }

            constexpr static Scalar compute2(const value_type& v) noexcept
            {
                return dot(v, v);
            }
        };

        template<typename Scalar>
        struct compute_length<Scalar, 1u>
        {
            static constexpr size_type size = 1u;
            using value_type = vector<Scalar, size>;

            static Scalar compute(const value_type& v) noexcept
            {
                return v[0];
            }

            constexpr static Scalar compute2(const value_type& v) noexcept
            {
                return v[0] * v[0];
            }
        };
    }

    template<typename vec_type, ::std::enable_if_t<is_vector_v<vec_type>, bool> = true>
    auto length(const vec_type& v) noexcept
    {
        return detail::compute_length<typename vec_type::scalar, vec_type::size>::compute(v);
    }

    template<typename vec_type, ::std::enable_if_t<is_vector_v<vec_type>, bool> = true>
    constexpr auto length2(const vec_type& v) noexcept
    {
        return detail::compute_length<typename vec_type::scalar, vec_type::size>::compute2(v);
    }
}
