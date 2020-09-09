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

        /** @brief given a PhysicsCollider CheckCollision calls "CheckCollisionWith". Both colliders are then passed through
         * to the correct "CheckCollisionWith" function with double dispatch.
         *  @param physicsCollider : the collider we would like to check collision against
         *  @param [in/out] manifold : a manifold that will hold the information
        */
        virtual void CheckCollision(
            std::shared_ptr<PhysicsCollider> physicsCollider, physics_manifold* manifold) {};

        /** @brief given a convexCollider checks if this collider collides the convexCollider. The information
         * the information is then passed to the manifold.
        */
        virtual void CheckCollisionWith(ConvexCollider* convexCollider, physics_manifold* manifold) {};


    protected:

        physicsComponent * physicsComponentOwner;



    private:
       


    };
}



