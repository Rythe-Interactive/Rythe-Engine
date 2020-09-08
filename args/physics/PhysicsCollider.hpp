#pragma once

#include <application/application.hpp>
#include <memory>

namespace args::physics
{
    struct physicsComponent;
    struct physics_manifold;
    class ConvexCollider;

    class PhysicsCollider
    {
    public:

        PhysicsCollider(physicsComponent* colliderOwner);

        virtual void CheckCollision(
            std::shared_ptr<PhysicsCollider> physicsCollider, physics_manifold* manifold) {};

        virtual void CheckCollisionWith(ConvexCollider* convexCollider, physics_manifold* manifold) {};


    protected:

        physicsComponent * physicsComponentOwner;



    private:
       


    };
}



