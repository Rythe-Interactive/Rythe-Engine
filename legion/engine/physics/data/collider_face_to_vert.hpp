#pragma once
#include <physics/halfedgeface.hpp>
#include <core/core.hpp>
namespace legion::physics
{
    struct ColliderFaceToVert
    {
        std::vector<math::vec3> outsideVerts;
        HalfEdgeFace* face = nullptr;

        ColliderFaceToVert(HalfEdgeFace* pFace,const math::vec3& initialVert) : face(pFace)
        {
            outsideVerts.push_back(initialVert);
        }
        
    };
}
