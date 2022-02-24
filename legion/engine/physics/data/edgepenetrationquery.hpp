#pragma once
#include <core/core.hpp>
#include <physics/data/penetrationquery.hpp>
#include <physics/halfedgeedge.hpp>

namespace legion::physics
{
    class EdgePenetrationQuery : public PenetrationQuery
    {
    public:
        HalfEdgeEdge* refEdge = nullptr;
        HalfEdgeEdge* incEdge = nullptr;

        EdgePenetrationQuery(HalfEdgeEdge* pRefEdge, HalfEdgeEdge* pIncEdge,
            math::float3& pFaceCentroid, math::float3& pNormal, float& pPenetration, bool pIsARef);

        virtual void populateContactList(physics_manifold& manifold,
            math::float4x4& refTransform, math::float4x4 incTransform,  PhysicsCollider* refCollider);
    };
}

