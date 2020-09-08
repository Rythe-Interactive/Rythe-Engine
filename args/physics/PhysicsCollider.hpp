#pragma once

#include <application/application.hpp>
#include <memory>

namespace args::physics
{
    struct physicsComponent;
    struct physicsManifold;
    class ConvexCollider;

    class PhysicsCollider
    {
    public:

        PhysicsCollider(physicsComponent* colliderOwner);

        virtual void CheckCollision(
            std::shared_ptr<PhysicsCollider> physicsCollider, physicsManifold* manifold) {};

        virtual void CheckCollisionWith(ConvexCollider* convexCollider, physicsManifold* manifold) {};


    protected:

        physicsComponent * physicsComponentOwner;



    private:
       


    };
}



