#pragma once
#include <core/math/math.hpp>

namespace args::core
{
    struct position : public math::vec3 {};

    struct rotation : public math::quat {};

    struct scale : public math::vec3 {};
}
