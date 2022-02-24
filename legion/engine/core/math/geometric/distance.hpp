#pragma once
#include <core/math/arithmetic/subtraction.hpp>
#include <core/math/geometric/length.hpp>

namespace legion::core::math
{
    template<typename vec_type0, typename vec_type1, std::enable_if_t<is_vector_v<vec_type0>&& is_vector_v<vec_type1>, bool> = true>
    auto dist(const vec_type0& a, const vec_type1& b) noexcept
    {
        return length(a - b);
    }

    template<typename vec_type0, typename vec_type1, std::enable_if_t<is_vector_v<vec_type0>&& is_vector_v<vec_type1>, bool> = true>
    constexpr auto dist2(const vec_type0& a, const vec_type1& b) noexcept
    {
        return length2(a - b);
    }

    template<typename vec_type0, typename vec_type1, std::enable_if_t<is_vector_v<vec_type0>&& is_vector_v<vec_type1>, bool> = true>
    auto distance(const vec_type0& a, const vec_type1& b) noexcept
    {
        return length(a - b);
    }

    template<typename vec_type0, typename vec_type1, std::enable_if_t<is_vector_v<vec_type0>&& is_vector_v<vec_type1>, bool> = true>
    constexpr auto distance2(const vec_type0& a, const vec_type1& b) noexcept
    {
        return length2(a - b);
    }
}
