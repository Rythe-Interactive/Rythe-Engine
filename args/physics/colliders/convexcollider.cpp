#include <physics/colliders/convexcollider.hpp>
#include <physics/physics_statics.hpp>
#include <physics/systems/physicssystem.hpp>
#include <physics/data/convexconvexpenetrationquery.h>
#include <physics/data/edgepenetrationquery.h>


namespace args::physics
{
    void ConvexCollider::CheckCollisionWith(ConvexCollider* convexCollider, physics_manifold& manifold) 
    {

        //--------------------- Check for a collision by going through the edges and faces of both polyhedrons  --------------//
        //'this' is colliderB and 'convexCollider' is colliderA

        HalfEdgeFace** ARefFace = &convexCollider->GetHalfEdgeFaces().at(0);
        float ARefSeperation;
        math::vec3 a;
        if (PhysicsStatics::FindSeperatingAxisByExtremePointProjection(this, convexCollider, manifold.transformB,manifold.transformA,  ARefFace, ARefSeperation, a))
        {
            manifold.isColliding = false;
            return;
        }

        HalfEdgeFace** BRefFace = &this->GetHalfEdgeFaces().at(0);
        float BRefSeperation;
        math::vec3 b;
        if (PhysicsStatics::FindSeperatingAxisByExtremePointProjection(convexCollider, this, manifold.transformA, manifold.transformB, BRefFace, BRefSeperation, b))
        {
            manifold.isColliding = false;
            return;
        }

        HalfEdgeEdge** edgeRef = &convexCollider->GetHalfEdgeFaces().at(0)->startEdge;
        HalfEdgeEdge** edgeInc = &this->GetHalfEdgeFaces().at(0)->startEdge;
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

        math::vec3 worldFaceCentroidA = manifold.transformA * math::vec4((*ARefFace)->centroid, 1);
        math::vec3 worldFaceNormalA = manifold.transformA * math::vec4((*ARefFace)->normal, 0);
        
        math::vec3 worldFaceCentroidB = manifold.transformB * math::vec4((*BRefFace)->centroid, 1);
        math::vec3 worldFaceNormalB = manifold.transformB * math::vec4((*BRefFace)->normal, 0);

        HalfEdgeEdge* refEdge = *edgeRef;
        math::vec3 worldEdgeAPosition = manifold.transformA * math::vec4(*refEdge->edgePositionPtr, 1);
        math::vec3 worldEdgeNormal = -edgeNormal;

        auto abPenetrationQuery =
            std::make_shared< ConvexConvexPenetrationQuery>(*ARefFace,*BRefFace, worldFaceCentroidA,worldFaceNormalA,ARefSeperation,true);

        auto baPenetrationQuery =
            std::make_shared < ConvexConvexPenetrationQuery>(*BRefFace, *ARefFace, worldFaceCentroidB, worldFaceNormalB, BRefSeperation, false);

        auto abEdgePenetrationQuery = 
            std::make_shared < EdgePenetrationQuery>(*edgeRef,*edgeInc,worldEdgeAPosition,worldEdgeNormal,aToBEdgeSeperation, false);

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

