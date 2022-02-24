#pragma once
#include <core/math/geometric/dot.hpp>
#include <core/math/exponential/inverse_sqrt.hpp>

namespace legion::core::math
{
    template<typename vec_type0, typename vec_type1, std::enable_if_t<is_vector_v<vec_type0>&& is_vector_v<vec_type1>, bool> = true>
    L_ALWAYS_INLINE static auto normalize_dot(const vec_type0& a, const vec_type1& b) noexcept
    {
        return dot(a, b) / sqrt(dot(a, a) * dot(b, b));
    }
}
