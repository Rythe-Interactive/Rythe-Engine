#include "ConvexCollider.h"
#include "Manifold.hpp"


namespace args::physics
{
    ConvexCollider::ConvexCollider(physicsComponent* colliderOwner) : PhysicsCollider(colliderOwner)
    {

    }

    void ConvexCollider::CheckCollision(const std::shared_ptr<PhysicsCollider> physicsComponent, physicsManifold* manifold)
    {
        physicsComponent->CheckCollisionWith(this, manifold);
    }

    void ConvexCollider::CheckCollisionWith(ConvexCollider* convexCollider, physicsManifold* manifold)
    {
    }


}

