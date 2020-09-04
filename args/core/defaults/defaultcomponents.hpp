#pragma once
#include <core/math/math.hpp>
#include <core/ecs/archetype.hpp>

namespace args::core
{
    struct position : public math::vec3 {};

    struct rotation : public math::quat
    {
        rotation() = default;

        rotation& operator=(const math::quat& other)
        {
            data = other.data;
            return *this;
        }
    };

    struct scale : public math::vec3
    {
        scale()
        {
            x = 1;
            y = 1;
            z = 1;
        }
    };

    struct transform : public ecs::archetype<position, rotation, scale> {};
}
