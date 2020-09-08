#include <physics/convexcollider.hpp>
#include <physics/physicsmanifold.hpp>


namespace args::physics
{
    ConvexCollider::ConvexCollider(physicsComponent* colliderOwner) : PhysicsCollider(colliderOwner)
    {

    }

    void ConvexCollider::CheckCollision(const std::shared_ptr<PhysicsCollider> physicsComponent, physics_manifold* manifold)
    {
        physicsComponent->CheckCollisionWith(this, manifold);
    }

    void ConvexCollider::CheckCollisionWith(ConvexCollider* convexCollider, physics_manifold* manifold)
    {

    }

    void ConvexCollider::ConstructBoxWithMesh()
    {

    }

    void ConvexCollider::CreateBox(const cube_collider_params& cubeParams)
    {
    }


}

