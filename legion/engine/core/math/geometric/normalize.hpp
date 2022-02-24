#pragma once
#include <cmath>

#include <core/types/primitives.hpp>
#include <core/math/vector/vector.hpp>
#include <core/math/arithmetic/division.hpp>
#include <core/math/geometric/length.hpp>

namespace legion::core::math
{
    namespace detail
    {
        template<typename Scalar, size_type Size>
        struct compute_normalize
        {
            static constexpr size_type size = Size;
            using value_type = vector<Scalar, size>;

            L_ALWAYS_INLINE static value_type compute(const value_type& v) noexcept
            {
                return v / length(v);
            }
        };

        template<typename Scalar>
        struct compute_normalize<Scalar, 1u>
        {
            static constexpr size_type size = 1u;
            using value_type = vector<Scalar, size>;

            L_ALWAYS_INLINE static Scalar compute(const value_type& v) noexcept
            {
                return static_cast<Scalar>(1);
            }
        };
    }

    template<typename vec_type, ::std::enable_if_t<is_vector_v<vec_type>, bool> = true>
    constexpr auto normalize(const vec_type& v) noexcept
    {
        return detail::compute_normalize<typename vec_type::scalar, vec_type::size>::compute(v);
    }
}
