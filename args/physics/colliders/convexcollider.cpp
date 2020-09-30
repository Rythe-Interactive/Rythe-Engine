#include <physics/colliders/convexcollider.hpp>
#include <physics/physics_statics.hpp>

#include <physics/data/convexconvexpenetrationquery.h>
#include <physics/data/edgepenetrationquery.h>

namespace args::physics
{
    void ConvexCollider::CheckCollisionWith(ConvexCollider* convexCollider, physics_manifold& manifold) 
    {

        //--------------------- Check for a collision by going through the edges and faces of both polyhedrons  --------------//
        //'this' is colliderA and 'convexCollider' is colliderB

        HalfEdgeFace** ARefFace = &convexCollider->GetHalfEdgeFaces().at(0);
        float ARefSeperation;

        if (PhysicsStatics::FindSeperatingAxisByExtremePointProjection(this, convexCollider, manifold.transformA, manifold.transformB, ARefFace, ARefSeperation))
        {
            manifold.isColliding = false;
            return;
        }

        HalfEdgeFace** BRefFace = &convexCollider->GetHalfEdgeFaces().at(0);
        float BRefSeperation;

        if (PhysicsStatics::FindSeperatingAxisByExtremePointProjection(convexCollider, this, manifold.transformB, manifold.transformA, BRefFace, BRefSeperation))
        {
            manifold.isColliding = false;
            return;
        }

        HalfEdgeEdge** edgeRef = &convexCollider->GetHalfEdgeFaces().at(0)->startEdge;
        HalfEdgeEdge** edgeInc = &this->GetHalfEdgeFaces().at(0)->startEdge;
        math::vec3 edgeNormal;
        float aToBEdgeSeperation;

        if (PhysicsStatics::FindSeperatingAxisByGaussMapEdgeCheck(this, convexCollider, manifold.transformA, manifold.transformB,
            edgeRef, edgeInc, edgeNormal, aToBEdgeSeperation))
        {
            manifold.isColliding = false;
            return;
        }

        //gets the value of edgePositionPtr. HalfEdgeEdge is stored as a pointer to a pointer,
        //it has a variable edgePositionPtr which is a pointer to a math::vec3.
        auto edgeposgetter = [](HalfEdgeEdge** p) {
            return *(*p)->edgePositionPtr;
        };

        //--------------------- A Collision has been found, find the most shallow penetration  -------------------------------------//

        //Get world position and normal of reference faces //

        math::vec3 worldFaceCentroidA = manifold.transformA * math::vec4((*ARefFace)->centroid, 1);
        math::vec3 worldFaceNormalA = manifold.transformA * math::vec4((*ARefFace)->centroid, 0);
        
        math::vec3 worldFaceCentroidB = manifold.transformB * math::vec4((*BRefFace)->centroid, 1);
        math::vec3 worldFaceNormalB = manifold.transformB * math::vec4((*BRefFace)->centroid, 0);

        math::vec3 worldEdgeAPosition = manifold.transformA * math::vec4(edgeposgetter(edgeRef), 1);
        math::vec3 worldEdgeNormal = manifold.transformA * math::vec4(edgeposgetter(edgeRef), 0);

        auto abPenetrationQuery =
            std::make_shared< ConvexConvexPenetrationQuery>(*ARefFace,*BRefFace, worldFaceCentroidA,worldFaceNormalA,ARefSeperation,true);

        auto baPenetrationQuery =
            std::make_shared < ConvexConvexPenetrationQuery>(*BRefFace, *ARefFace, worldFaceCentroidB, worldFaceNormalB, BRefSeperation, false);

        auto abEdgePenetrationQuery = 
            std::make_shared < EdgePenetrationQuery>(*edgeRef,*edgeInc,worldEdgeAPosition,worldEdgeNormal,aToBEdgeSeperation,true);

        std::array<std::shared_ptr<PenetrationQuery>, 3> penetrationQueryArray{ abEdgePenetrationQuery ,abPenetrationQuery, baPenetrationQuery };

        auto lessThan
        {
            []( std::shared_ptr<PenetrationQuery> lhs,  std::shared_ptr<PenetrationQuery> rhs)
            {
            return lhs->penetration < rhs->penetration;
            }
        };

        manifold.penetrationInformation = *std::max_element(penetrationQueryArray.begin(), penetrationQueryArray.end(), lessThan);

        manifold.isColliding = true;

        log::debug("No seperating axis found!");
        TempLine line;
        line.start = manifold.transformA[3];
        line.end = manifold.transformB[3];

        //convexCollider->manifoldsFound.push_back(manifold.penetrationInformation);

        collisionsFound.push_back(line);
    }




}

