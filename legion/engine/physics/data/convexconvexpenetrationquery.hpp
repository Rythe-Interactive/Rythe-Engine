#pragma once
#include <physics/data/penetrationquery.hpp>
#include <physics/halfedgeedge.hpp>

namespace legion::physics
{
    class ConvexConvexPenetrationQuery : public PenetrationQuery
    {
    public:

        HalfEdgeFace* refFace = nullptr;
        HalfEdgeFace* incFace = nullptr;

        ConvexConvexPenetrationQuery(HalfEdgeFace* pRefFace, HalfEdgeFace* pIncFace, math::float3& pFaceCentroid, math::float3& pNormal, float pPenetration, bool pIsARef);

        virtual void populateContactList(physics_manifold& manifold,  math::float4x4& refTransform,
            math::float4x4 incTransform , PhysicsCollider* refCollider) override;

    };
}


