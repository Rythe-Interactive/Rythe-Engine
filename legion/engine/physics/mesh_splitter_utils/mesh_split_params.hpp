#pragma once

#include <core/core.hpp>

namespace legion::physics
{
    struct MeshSplitParams
    {
        MeshSplitParams(math::vec3 pPlanePosition,math::vec3 pPlaneNormal)
            : planePostion(pPlanePosition), planeNormal(pPlaneNormal)
        {

        }

        math::vec3 planePostion;
        math::vec3 planeNormal;
    };
}


