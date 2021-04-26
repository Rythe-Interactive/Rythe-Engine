#pragma once
#include <core/core.hpp>

namespace legion::physics
{
    struct FractureCountdown
    {
        float fractureTime = FLT_MAX;
        float fractureStrength;
        math::vec3 explosionPoint;
        bool explodeNow = false;
    };
}
