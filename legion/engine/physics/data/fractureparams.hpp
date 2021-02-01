#pragma once
#include <core/core.hpp>

namespace legion::physics
{
    struct FractureParams
    {
        
        math::vec3 explosionCentroid;
        float strength;

        FractureParams(math::vec3 pWorldExplostionCent, float pStrength = 5.0f)
            : explosionCentroid(pWorldExplostionCent),strength(pStrength)
        {

        }
    };
}

