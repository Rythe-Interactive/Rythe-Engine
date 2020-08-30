#pragma once
#include <core/math/math.hpp>
#include <core/ecs/archetype.hpp>

namespace args::core
{
    struct position : public math::vec3 {};

    struct rotation : public math::quat {};

    struct scale : public math::vec3 {};

    struct transform : public ecs::archetype<position, rotation, scale> {};
}
