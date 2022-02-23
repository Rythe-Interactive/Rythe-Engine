#include <physics/diviner/colliders/convexcollider.hpp>
#include <physics/diviner/physics_statics.hpp>
#include <physics/diviner/data/identifier.hpp>
#include <physics/diviner/data/convexconvexpenetrationquery.hpp>
#include <physics/diviner/data/edgepenetrationquery.hpp>
#include <physics/diviner/data/pointer_encapsulator.hpp>
#include <physics/diviner/systems/physicssystem.hpp>
#include <rendering/debugrendering.hpp>

namespace legion::physics
{
    void ConvexCollider::AddConvergenceIdentifier(const physics_contact& contact)
    {
        if (contact.label.IsSet())
        {
            convergenceIdentifiers.push_back(
                std::make_unique<ConvexConvergenceIdentifier>(contact.label, contact.totalLambda,
                    contact.tangent1Lambda, contact.tangent2Lambda, GetColliderID()));
        }
    }

    void ConvexCollider::CheckCollisionWith(ConvexCollider* convexCollider, physics_manifold& manifold)
    {
        OPTICK_EVENT();

        // Middle-phase collision detection
        // Do AABB collision to check whether collision is possible
        auto aabbThis = this->GetMinMaxWorldAABB();
        auto aabbOther = convexCollider->GetMinMaxWorldAABB();
        auto& [low0, high0] = aabbThis;
        auto& [low1, high1] = aabbOther;
       
        if (!physics::PhysicsStatics::CollideAABB(low0, high0, low1, high1))
        {
            manifold.isColliding = false;
            return;
        }

        //--------------------- Check for a collision by going through the edges and faces of both polyhedrons  --------------//
        //'this' is colliderB and 'convexCollider' is colliderA
        
        PointerEncapsulator < HalfEdgeFace> ARefFace;

        float ARefSeperation;
        if (PhysicsStatics::FindSeperatingAxisByExtremePointProjection(
            this, convexCollider, manifold.transformB,manifold.transformA,  ARefFace, ARefSeperation) || !ARefFace.ptr)
        {
            manifold.isColliding = false;
            return;
        }

        PointerEncapsulator < HalfEdgeFace> BRefFace;

        float BRefSeperation;
        if (PhysicsStatics::FindSeperatingAxisByExtremePointProjection(convexCollider,
            this, manifold.transformA, manifold.transformB, BRefFace, BRefSeperation) || !BRefFace.ptr)
        {
            manifold.isColliding = false;
            return;
        }

        PointerEncapsulator< HalfEdgeEdge> edgeRef;
        PointerEncapsulator< HalfEdgeEdge> edgeInc;

        math::vec3 edgeNormal;
        float aToBEdgeSeperation;

        if (PhysicsStatics::FindSeperatingAxisByGaussMapEdgeCheck( this, convexCollider, manifold.transformB, manifold.transformA,
            edgeRef, edgeInc, edgeNormal, aToBEdgeSeperation,true ) || !edgeRef.ptr )
        {
            manifold.isColliding = false;
            return;
        }

        //--------------------- A Collision has been found, find the most shallow penetration  ------------------------------------//

        //TODO all penetration querys should supply a constructor that takes in a  ConvexConvexCollisionInfo
        
        math::vec3 worldFaceCentroidA = manifold.transformA * math::vec4(ARefFace.ptr->centroid, 1);
        math::vec3 worldFaceNormalA = manifold.transformA * math::vec4(ARefFace.ptr->normal, 0);
        
        math::vec3 worldFaceCentroidB = manifold.transformB * math::vec4(BRefFace.ptr->centroid, 1);
        math::vec3 worldFaceNormalB = manifold.transformB * math::vec4(BRefFace.ptr->normal, 0);

        math::vec3 worldEdgeAPosition = edgeRef.ptr? manifold.transformB * math::vec4(edgeRef.ptr->edgePosition, 1) : math::vec3();
        math::vec3 worldEdgeNormal = edgeNormal;

        auto abPenetrationQuery =
            std::make_unique< ConvexConvexPenetrationQuery>(ARefFace.ptr
                , BRefFace.ptr, worldFaceCentroidA,worldFaceNormalA, ARefSeperation,true);

        auto baPenetrationQuery =
            std::make_unique < ConvexConvexPenetrationQuery>(BRefFace.ptr, ARefFace.ptr,
                worldFaceCentroidB, worldFaceNormalB, BRefSeperation, false);

        auto abEdgePenetrationQuery = 
            std::make_unique < EdgePenetrationQuery>(edgeRef.ptr, edgeInc.ptr,worldEdgeAPosition,worldEdgeNormal,
                aToBEdgeSeperation, false);

        //-------------------------------------- Choose which PenetrationQuery to use for contact population --------------------------------------------------//

        if (abPenetrationQuery->penetration + physics::constants::faceToFacePenetrationBias >
            baPenetrationQuery->penetration)
        {
            manifold.penetrationInformation = std::move(abPenetrationQuery);
        }
        else
        {
            manifold.penetrationInformation = std::move(baPenetrationQuery);
        }


        if (abEdgePenetrationQuery->penetration >
            manifold.penetrationInformation->penetration + physics::constants::faceToEdgePenetrationBias)
        {
            manifold.penetrationInformation = std::move(abEdgePenetrationQuery);
        }

        manifold.isColliding = true;
    }

    void ConvexCollider::PopulateContactPointsWith(ConvexCollider* convexCollider, physics_manifold& manifold)
    {
        OPTICK_EVENT();
        math::mat4& refTransform = manifold.penetrationInformation->isARef ? manifold.transformA : manifold.transformB;
        math::mat4& incTransform = manifold.penetrationInformation->isARef ? manifold.transformB : manifold.transformA;

        dvrInternalPhysicsComponent* refPhysicsComp = manifold.penetrationInformation->isARef ? manifold.physicsCompA : manifold.physicsCompB;
        dvrInternalPhysicsComponent* incPhysicsComp = manifold.penetrationInformation->isARef ? manifold.physicsCompB : manifold.physicsCompA;

        PhysicsCollider* refCollider = manifold.penetrationInformation->isARef ? manifold.colliderA : manifold.colliderB;
        PhysicsCollider* incCollider = manifold.penetrationInformation->isARef ? manifold.colliderB : manifold.colliderA;

        manifold.penetrationInformation->populateContactList(manifold, refTransform, incTransform, refCollider);

        dvrInternalRigidbody* refRB = manifold.penetrationInformation->isARef ? manifold.dvrInternalRigidbodyA : manifold.dvrInternalRigidbodyB;
        dvrInternalRigidbody* incRB = manifold.penetrationInformation->isARef ? manifold.dvrInternalRigidbodyB : manifold.dvrInternalRigidbodyA;

        math::vec3 refWorldCentroid = refTransform * math::vec4(refPhysicsComp->localCenterOfMass,1);
        math::vec3 incWorldCentroid = incTransform * math::vec4(incPhysicsComp->localCenterOfMass,1);

        for ( auto& contact : manifold.contacts)
        {
            contact.incTransform = incTransform;
            contact.refTransform = refTransform;

            contact.rbInc = incRB;
            contact.rbRef = refRB;
           
            contact.collisionNormal = manifold.penetrationInformation->normal;

            contact.refRBCentroid = refWorldCentroid;
            contact.incRBCentroid = incWorldCentroid;

        }
    }

    void ConvexCollider::UpdateTightAABB(const math::mat4& transform)
    {
        minMaxWorldAABB = PhysicsStatics::ConstructAABBFromTransformedVertices
        (vertices, transform);
    }

    void ConvexCollider::DrawColliderRepresentation(const math::mat4& transform,math::color usedColor, float width, float time,bool ignoreDepth)
    {
        if (!shouldBeDrawn) { return; }

        for (auto face : GetHalfEdgeFaces())
        {
            physics::HalfEdgeEdge* initialEdge = face->startEdge;
            physics::HalfEdgeEdge* currentEdge = face->startEdge;

            math::vec3 faceStart = transform * math::vec4(face->centroid, 1);
            math::vec3 faceEnd = faceStart + math::vec3((transform * math::vec4(face->normal, 0))) * 0.5f;

            debug::user_projectDrawLine(faceStart, faceEnd, math::colors::green, 2.0f);

            if (!currentEdge) { return; }

            do
            {
                physics::HalfEdgeEdge* edgeToExecuteOn = currentEdge;
                currentEdge = currentEdge->nextEdge;

                math::vec3 worldStart = transform * math::vec4(edgeToExecuteOn->edgePosition, 1);
                math::vec3 worldEnd = transform * math::vec4(edgeToExecuteOn->nextEdge->edgePosition, 1);

                debug::user_projectDrawLine(worldStart, worldEnd, usedColor, width, time,ignoreDepth);

            } while (initialEdge != currentEdge && currentEdge != nullptr);
        }
    }

    void ConvexCollider::populateVertexListWithHalfEdges()
    {
        auto& verticesVec = vertices;

        const size_t reserveSize = halfEdgeFaces.size() * 3;
        verticesVec.reserve(reserveSize);

        auto collectVertices = [&verticesVec](HalfEdgeEdge* edge)
        {
            edge->calculateRobustEdgeDirection();
            verticesVec.push_back(edge->edgePosition -= PhysicsStatics::PointDistanceToPlane(edge->face->normal, edge->face->centroid, edge->edgePosition));
        };

        for (auto face : halfEdgeFaces)
        {
            face->forEachEdge(collectVertices);
        }
    }
}

