#include <physics/colliders/convexcollider.hpp>
#include <physics/physics_statics.hpp>
#include <physics/systems/physicssystem.hpp>
#include <physics/data/convexconvexpenetrationquery.h>
#include <physics/data/edgepenetrationquery.h>
#include <physics/data/pointer_encapsulator.hpp>

namespace args::physics
{
    void ConvexCollider::CheckCollisionWith(ConvexCollider* convexCollider, physics_manifold& manifold) 
    {

        //--------------------- Check for a collision by going through the edges and faces of both polyhedrons  --------------//
        //'this' is colliderB and 'convexCollider' is colliderA

        PointerEncapsulator < HalfEdgeFace> ARefFace;

        float ARefSeperation;
        math::vec3 a;
        if (PhysicsStatics::FindSeperatingAxisByExtremePointProjection(this, convexCollider, manifold.transformB,manifold.transformA,  ARefFace, ARefSeperation, a))
        {
            manifold.isColliding = false;
            return;
        }

        PointerEncapsulator < HalfEdgeFace> BRefFace;
      
        float BRefSeperation;
        math::vec3 b;
        if (PhysicsStatics::FindSeperatingAxisByExtremePointProjection(convexCollider, this, manifold.transformA, manifold.transformB, BRefFace, BRefSeperation, b))
        {
            manifold.isColliding = false;
            return;
        }

        PointerEncapsulator< HalfEdgeEdge> edgeRef;
        PointerEncapsulator< HalfEdgeEdge> edgeInc;

        //edgeRef.ptr = 

        math::vec3 edgeNormal;
        float aToBEdgeSeperation;

        if (PhysicsStatics::FindSeperatingAxisByGaussMapEdgeCheck(this, convexCollider, manifold.transformB, manifold.transformA,
            edgeRef, edgeInc, edgeNormal, aToBEdgeSeperation))
        {
            manifold.isColliding = false;
            return;
        }

        //assert(*edgeRef);


        //--------------------- A Collision has been found, find the most shallow penetration  ------------------------------------//

        //Get world position and normal of reference faces //

        math::vec3 worldFaceCentroidA = manifold.transformA * math::vec4((ARefFace.ptr)->centroid, 1);
        math::vec3 worldFaceNormalA = manifold.transformA * math::vec4((ARefFace.ptr)->normal, 0);
        
        math::vec3 worldFaceCentroidB = manifold.transformB * math::vec4((BRefFace.ptr)->centroid, 1);
        math::vec3 worldFaceNormalB = manifold.transformB * math::vec4((BRefFace.ptr)->normal, 0);

    
        math::vec3 worldEdgeAPosition = edgeRef.ptr? manifold.transformA * math::vec4(*edgeRef.ptr->edgePositionPtr, 1) : math::vec3();
        math::vec3 worldEdgeNormal = -edgeNormal;

        auto abPenetrationQuery =
            std::make_shared< ConvexConvexPenetrationQuery>(ARefFace.ptr,BRefFace.ptr, worldFaceCentroidA,worldFaceNormalA,ARefSeperation,true);

        auto baPenetrationQuery =
            std::make_shared < ConvexConvexPenetrationQuery>(BRefFace.ptr, ARefFace.ptr, worldFaceCentroidB, worldFaceNormalB, BRefSeperation, false);

        auto abEdgePenetrationQuery = 
            std::make_shared < EdgePenetrationQuery>(edgeRef.ptr,edgeInc.ptr,worldEdgeAPosition,worldEdgeNormal,aToBEdgeSeperation, false);

        std::array<std::shared_ptr<PenetrationQuery>, 3> penetrationQueryArray{ abEdgePenetrationQuery, abPenetrationQuery, baPenetrationQuery  };

        auto lessThan
        {
            []( std::shared_ptr<PenetrationQuery> lhs,  std::shared_ptr<PenetrationQuery> rhs)
            {
            return lhs->penetration < rhs->penetration;
            }
        };

        manifold.penetrationInformation = *std::max_element(penetrationQueryArray.begin(), penetrationQueryArray.end(),lessThan);

        manifold.isColliding = true;
        TempLine line;
        line.start = manifold.transformA[3];
        line.end = manifold.transformB[3];

        PhysicsSystem::penetrationQueries.push_back(manifold.penetrationInformation);
        PhysicsSystem::aPoint.push_back(a);
        PhysicsSystem::bPoint.push_back(b);

        collisionsFound.push_back(line);
    }

    void ConvexCollider::PopulateContactPointsWith(ConvexCollider* convexCollider, physics_manifold& manifold)
    {
        log::debug("ConvexCollider::PopulateContactPointsWith ");

        math::mat4& refTransform = manifold.penetrationInformation->isARef ? manifold.transformA : manifold.transformB;
        math::mat4& incTransform = manifold.penetrationInformation->isARef ? manifold.transformB : manifold.transformA;

        manifold.penetrationInformation->populateContactList(manifold,refTransform,incTransform);

    }




}

