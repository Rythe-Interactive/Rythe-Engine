#pragma once
#include <core/math/math.hpp>
#include <core/ecs/archetype.hpp>

namespace args::core
{
    struct position : public math::vec3
    {
        position() = default;
        position(const position&) = default;
        position(position&&) = default;
        position(const math::vec3& src) : math::vec3(src) {}
        position& operator=(const position&) = default;
        position& operator=(position&&) = default;
        position& operator=(const math::vec3& src)
        {
            data = src.data;
            return *this;
        }
    };

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
        scale() : math::vec3(1, 1, 1) {}
        scale(const scale&) = default;
        scale(scale&&) = default;
        scale(const math::vec3 & src) : math::vec3(src) {}
        scale& operator=(const scale&) = default;
        scale& operator=(scale&&) = default;
        scale& operator=(const math::vec3 & src)
        {
            data = src.data;
            return *this;
        }
    };

    struct transform : public ecs::archetype<position, rotation, scale> {};
}
