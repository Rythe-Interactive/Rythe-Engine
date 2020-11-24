#pragma once
#include <core/core.hpp>

namespace legion::physics
{
    struct MeshHalfEdge
    {
        math::vec3 position;
        std::shared_ptr<MeshHalfEdge> nextEdge;
        std::shared_ptr<MeshHalfEdge> pairingEdge;

        bool isVisited = false;

        MeshHalfEdge(math::vec3 pPosition) : position(pPosition)
        {

        };



    };


}





