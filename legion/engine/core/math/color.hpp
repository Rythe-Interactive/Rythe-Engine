#pragma once
#include <core/math/glm/glm_include.hpp>

/**
 * @file color.hpp
 */

namespace legion::core::math
{
    /**@class color
     * @brief Color oriented wrapper around vec4.
     * @ref legion::core::math::vec4
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
        const color lightgrey   { 0.75f, 0.75f, 0.75f, 1.f };
        const color grey        { 0.5f, 0.5f, 0.5f, 1.f };
        const color darkgrey    { 0.25f, 0.25f, 0.25f, 1.f };
        const color red         { 1.f, 0.f, 0.f, 1.f };
        const color green       { 0.f, 1.f, 0.f, 1.f };
        const color blue        { 0.f, 0.f, 1.f, 1.f };
        const color yellow      { 1.f, 1.f, 0.f, 1.f };
        const color cyan        { 0.f, 1.f, 1.f, 1.f };
        const color magenta     { 1.f, 0.f, 1.f, 1.f };
        const color orange      { 1.0f, 0.584f, 0.f, 1.f }; // (255, 149, 0, 1)
        const color cornflower  { 0.392f, 0.584f, 0.929f, 1.0f };
    }
}

#if !defined(DOXY_EXCLUDE)
namespace std
{
    template<>
    struct hash<legion::core::math::color>
    {
        std::size_t operator()(legion::core::math::color const& color) const noexcept
        {
            size_t seed = 0;
            hash<float> hasher;
            legion::core::math::detail::hash_combine(seed, hasher(color.r));
            legion::core::math::detail::hash_combine(seed, hasher(color.g));
            legion::core::math::detail::hash_combine(seed, hasher(color.b));
            legion::core::math::detail::hash_combine(seed, hasher(color.a));
            return seed;
        }
    };
}
#endif
