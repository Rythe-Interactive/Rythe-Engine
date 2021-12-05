#pragma once
#include <core/math/vector/vector.hpp>

/**
 * @file color.hpp
 */

namespace legion::core::math
{
    /**@class color
     * @brief Color oriented wrapper around float4.
     * @ref legion::core::math::float4
     */
    using color = float4;

    /**@class highp_color
     * @brief Double precision version of legion::core::math::color.
     * @ref legion::core::math::double4
     */
    using dcolor = double4;

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
        const color orange      { 1.0f, 0.584f, 0.f, 1.f };
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
