#pragma once

#include <core/core.hpp>

namespace legion::physics
{
    struct MeshSplitParams
    {
        MeshSplitParams(math::float3 pPlanePosition,math::float3 pPlaneNormal)
            : planePostion(pPlanePosition), planeNormal(pPlaneNormal)
        {

        }

        math::float3 planePostion;
        math::float3 planeNormal;
    };
}


