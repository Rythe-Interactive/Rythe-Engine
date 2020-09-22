#pragma once
#include <core/math/glm/glm_include.hpp>

/**
 * @file color.hpp
 */

namespace args::core::math
{
    /**@class color
     * @brief Color oriented wrapper around vec4.
     * @ref args::core::math::vec4
     */
    struct color : public vec4
    {
        color(const vec4& s) : vec4(s) {}
        color(vec4&& s) :vec4(s) {}
        color(float r, float g, float b, float a = 1.f) : vec4(r, g, b, a) {}
        color() = default;

        bool operator==(const color& other) const
        {
            return r == other.x && g == other.g && b == other.b && a == other.a;
        }

        bool operator==(const vec4& other) const
        {
            return r == other.x && g == other.g && b == other.b && a == other.a;
        }
    };

    /**
     * @brief Default colors in RGBA.
     */
    namespace colors
    {
        const color black       { 0.f, 0.f, 0.f, 1.f };
        const color transparent { 0.f, 0.f, 0.f, 0.f };
        const color white       { 1.f, 1.f, 1.f, 1.f };
        const color red         { 1.f, 0.f, 0.f, 1.f };
        const color green       { 0.f, 1.f, 0.f, 1.f };
        const color blue        { 0.f, 0.f, 1.f, 1.f };
        const color yellow      { 1.f, 1.f, 0.f, 1.f };
        const color cyan        { 0.f, 1.f, 1.f, 1.f };
        const color magenta     { 1.f, 0.f, 1.f, 1.f };
    }
}

namespace std
{
    template<>
    struct hash<args::core::math::color>
    {
        std::size_t operator()(args::core::math::color const& color) const noexcept
        {
            size_t seed = 0;
            hash<float> hasher;
            args::core::math::detail::hash_combine(seed, hasher(color.r));
            args::core::math::detail::hash_combine(seed, hasher(color.g));
            args::core::math::detail::hash_combine(seed, hasher(color.b));
            args::core::math::detail::hash_combine(seed, hasher(color.a));
            return seed;
        }
    };
}
