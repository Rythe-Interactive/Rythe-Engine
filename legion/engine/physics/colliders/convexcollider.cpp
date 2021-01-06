#include <physics/colliders/convexcollider.hpp>
#include <physics/physics_statics.hpp>
#include <physics/data/identifier.hpp>
#include <physics/data/convexconvexpenetrationquery.hpp>
#include <physics/data/edgepenetrationquery.hpp>
#include <physics/data/pointer_encapsulator.hpp>
#include <physics/systems/physicssystem.hpp>

namespace legion::physics
{
    void ConvexCollider::CheckCollisionWith(ConvexCollider* convexCollider, physics_manifold& manifold) 
    {
        auto compIDA = manifold.physicsCompA.entity.get_component_handle<identifier>();
        auto compIDB = manifold.physicsCompB.entity.get_component_handle<identifier>();

        //--------------------- Check for a collision by going through the edges and faces of both polyhedrons  --------------//
        //'this' is colliderB and 'convexCollider' is colliderA
        

        log::debug("-------------------- SAT CHECK -----------------");
        PointerEncapsulator < HalfEdgeFace> ARefFace;

        float ARefSeperation;
        if (PhysicsStatics::FindSeperatingAxisByExtremePointProjection(
            this, convexCollider, manifold.transformB,manifold.transformA,  ARefFace, ARefSeperation, math::colors::cyan) || !ARefFace.ptr)
        {
            //log::debug("-> Seperating axis not found on A {}", ARefSeperation);
            //ARefFace.ptr->DEBUG_DrawFace(manifold.transformA, math::colors::magenta,1.0f);
            manifold.isColliding = false;
            return;
        }

        //log::debug("-> ARefSeperation Seperating Axis not Found, {} ", ARefSeperation);
        //ARefFace.ptr->DEBUG_DrawFace(manifold.transformA, math::colors::cyan,5.0f);
        

        PointerEncapsulator < HalfEdgeFace> BRefFace;
      
        float BRefSeperation;
        if (PhysicsStatics::FindSeperatingAxisByExtremePointProjection(convexCollider, this, manifold.transformA, manifold.transformB, BRefFace, BRefSeperation, math::colors::orange) || !BRefFace.ptr)
        {
            //log::debug("-> Seperating axis not found on B {} ", BRefSeperation);
            //BRefFace.ptr->DEBUG_DrawFace(manifold.transformB, math::colors::magenta);
            manifold.isColliding = false;
            return;
        }

        //log::debug("-> BRefSeperation Seperating Axis not Found, {}", BRefSeperation);
        //BRefFace.ptr->DEBUG_DrawFace(manifold.transformB, math::colors::orange, 5.0f);

        PointerEncapsulator< HalfEdgeEdge> edgeRef;
        PointerEncapsulator< HalfEdgeEdge> edgeInc;

        math::vec3 edgeNormal;
        float aToBEdgeSeperation;

        if (PhysicsStatics::FindSeperatingAxisByGaussMapEdgeCheck(this, convexCollider, manifold.transformB, manifold.transformA,
            edgeRef, edgeInc, edgeNormal, aToBEdgeSeperation))
        {
            //log::debug("-> Seperating axis found on edges {} ", aToBEdgeSeperation);
    /*        edgeRef.ptr->DEBUG_drawEdge(manifold.transformB, math::colors::magenta,20.0f,10.0f);
            edgeInc.ptr->DEBUG_drawEdge(manifold.transformA, math::colors::magenta, 20.0f, 10.0f);*/
            manifold.isColliding = false;
            return;
        }

        //edgeRef.ptr->DEBUG_drawEdge(manifold.transformB, math::colors::red);
        //edgeInc.ptr->DEBUG_drawEdge(manifold.transformA, math::colors::red);
        //--------------------- A Collision has been found, find the most shallow penetration  ------------------------------------//
        /*log::debug("ARefFace ", math::to_string(ARefFace.ptr->normal));
        log::debug("BRefFace ", math::to_string(BRefFace.ptr->normal));*/


        //Get world position and normal of reference faces //
        
        math::vec3 worldFaceCentroidA = manifold.transformA * math::vec4(ARefFace.ptr->centroid, 1);
        math::vec3 worldFaceNormalA = manifold.transformA * math::vec4(ARefFace.ptr->normal, 0);
        
        math::vec3 worldFaceCentroidB = manifold.transformB * math::vec4(BRefFace.ptr->centroid, 1);
        math::vec3 worldFaceNormalB = manifold.transformB * math::vec4(BRefFace.ptr->normal, 0);

    
        math::vec3 worldEdgeAPosition = edgeRef.ptr? manifold.transformB * math::vec4(edgeRef.ptr->edgePosition, 1) : math::vec3();
        math::vec3 worldEdgeNormal = edgeNormal;

        auto abPenetrationQuery =
            std::make_shared< ConvexConvexPenetrationQuery>(ARefFace.ptr,BRefFace.ptr, worldFaceCentroidA,worldFaceNormalA,ARefSeperation,true);

        auto baPenetrationQuery =
            std::make_shared < ConvexConvexPenetrationQuery>(BRefFace.ptr, ARefFace.ptr, worldFaceCentroidB, worldFaceNormalB, BRefSeperation, false);

        auto abEdgePenetrationQuery = 
            std::make_shared < EdgePenetrationQuery>(edgeRef.ptr,edgeInc.ptr,worldEdgeAPosition,worldEdgeNormal,aToBEdgeSeperation, false);

        std::array<std::shared_ptr<PenetrationQuery>, 3> penetrationQueryArray{ abEdgePenetrationQuery, abPenetrationQuery, baPenetrationQuery  };

        /*log::debug("abPenetrationQuery with normal {} , has penetration {}", abPenetrationQuery->normal,abPenetrationQuery->penetration);
        log::debug("baPenetrationQuery n with normal {} ,  has penetration {}", baPenetrationQuery->normal,baPenetrationQuery->penetration);
        log::debug("abEdgePenetrationQuery with normal {} ,  has penetration {}", abEdgePenetrationQuery->normal ,abEdgePenetrationQuery->penetration);*/

        //auto lessThan
        //{
        //    []( std::shared_ptr<PenetrationQuery> lhs,  std::shared_ptr<PenetrationQuery> rhs)
        //    {
        //    return lhs->penetration < rhs->penetration;
        //    }
        //};

        ////manifold.penetrationInformation = *std::max_element(penetrationQueryArray.begin(), penetrationQueryArray.end(),lessThan);

        if (abPenetrationQuery->penetration + physics::constants::faceToFacePenetrationBias >
            baPenetrationQuery->penetration)
        {
            manifold.penetrationInformation = abPenetrationQuery;
        }
        else
        {
            manifold.penetrationInformation = baPenetrationQuery;
        }


        if (abEdgePenetrationQuery->penetration >
            manifold.penetrationInformation->penetration + physics::constants::faceToEdgePenetrationBias)
        {
            manifold.penetrationInformation = abEdgePenetrationQuery;
        }

        manifold.isColliding = true;
        //log::debug("manifold.penetrationInformation chosen {}", manifold.penetrationInformation->debugID);

        //keeping this here so i can copy pasta when i need it again
        //log::debug("---- PENETRATION INFO");

        //log::debug("---- abPenetrationQuery {}",abPenetrationQuery->penetration);
        //log::debug("---- baPenetrationQuery {}",baPenetrationQuery->penetration);
        //log::debug("---- abEdgePenetrationQuery {}", abEdgePenetrationQuery->penetration);
        //log::debug("---- chosen penetration {}", manifold.penetrationInformation->penetration);

        //log::debug("Collision FOUND between {} and {}!" , compIDA.read().id, compIDB.read().id);

        physics::PhysicsSystem::penetrationQueries.push_back(manifold.penetrationInformation);
   
    }

    void ConvexCollider::PopulateContactPointsWith(ConvexCollider* convexCollider, physics_manifold& manifold)
    {
        math::mat4& refTransform = manifold.penetrationInformation->isARef ? manifold.transformA : manifold.transformB;
        math::mat4& incTransform = manifold.penetrationInformation->isARef ? manifold.transformB : manifold.transformA;

     

        auto refPhysicsCompHandle = manifold.penetrationInformation->isARef ? manifold.physicsCompA : manifold.physicsCompB;
        auto incPhysicsCompHandle = manifold.penetrationInformation->isARef ? manifold.physicsCompB : manifold.physicsCompA;

        auto refCollider = manifold.penetrationInformation->isARef ? manifold.colliderA : manifold.colliderB;
        auto incCollider = manifold.penetrationInformation->isARef ? manifold.colliderB : manifold.colliderA;

        manifold.penetrationInformation->populateContactList(manifold, refTransform, incTransform,refCollider);

        ecs::component_handle<rigidbody> refRB = refPhysicsCompHandle.entity.get_component_handle<rigidbody>();
        ecs::component_handle<rigidbody>  incRB = incPhysicsCompHandle.entity.get_component_handle<rigidbody>();

        math::vec3 refWorldCentroid = refTransform * math::vec4(refPhysicsCompHandle.read().localCenterOfMass,1);
        math::vec3 incWorldCentroid = incTransform * math::vec4(incPhysicsCompHandle.read().localCenterOfMass,1);

        for ( auto& contact : manifold.contacts)
        {
            contact.incTransform = incTransform;
            contact.refTransform = refTransform;

            contact.rbIncHandle = incRB;
            contact.rbRefHandle = refRB;
           
            contact.collisionNormal = manifold.penetrationInformation->normal;

            contact.refRBCentroid = refWorldCentroid;
            contact.incRBCentroid = incWorldCentroid;

        }
    }




}

