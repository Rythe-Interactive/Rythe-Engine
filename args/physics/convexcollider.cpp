#include <physics/convexcollider.hpp>
#include <physics/PhysicsStatics.h>

namespace args::physics
{
    void ConvexCollider::CheckCollisionWith(ConvexCollider* convexCollider, physics_manifold& manifold) 
    {
      

        //'this' is colliderA and 'convexCollider' is colliderB

        HalfEdgeFace* ARefFace = nullptr;
        float ARefSeperation;

        if (PhysicsStatics::FindSeperatingAxisByExtremePointProjection(this, convexCollider, manifold.transformA, manifold.transformB, ARefFace, ARefSeperation))
        {
            //log::debug("Seperating Axis Found With ConvexA as ref");
            return;
        }

        HalfEdgeFace* BRefFace = nullptr;
        float BRefSeperation;

        if (PhysicsStatics::FindSeperatingAxisByExtremePointProjection(convexCollider, this, manifold.transformB, manifold.transformA, BRefFace, BRefSeperation))
        {
            //log::debug("Seperating Axis Found With ConvexB as ref");
            return;
        }

        log::debug("No seperating axis found!");
        TempLine line;
        line.start = manifold.transformA[3];
        line.end = manifold.transformB[3];

        collisionsFound.push_back(line);
    }




}

