#pragma once

#include <core/core.hpp>

namespace legion::physics
{
    struct cube_collider_params
    {
        math::vec3 offset;
        float width;
        float breadth;
        float height;

        cube_collider_params(float pWidth = 2.0f, float pBreath = 2.0f
            , float pHeight = 2.0f, math::vec3 pOffset = math::vec3(0.0f))
            : width{pWidth},breadth{pBreath},height{pHeight},offset{pOffset}
        {
            
        }

    };
}
