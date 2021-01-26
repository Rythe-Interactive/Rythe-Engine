#pragma once

#include <physics/colliders/physicscollider.hpp>
#include <core/core.hpp>
#include <physics/physics_contact.hpp>
#include <core/core.hpp>
#include <physics/components/physics_component.hpp>
#include <physics/data/penetrationquery.hpp>

namespace legion::physics
{
    class PhysicsCollider;

    /** @struct physics_manifold
    * @brief contains the necessary information to detect and resolve a collision
    */
    struct physics_manifold
    {
        std::vector<physics_contact> contacts;

        math::mat4 transformA;
        math::mat4 transformB;

        PhysicsCollider* colliderA;
        PhysicsCollider* colliderB;

        ecs::entity_handle entityA;
        ecs::entity_handle entityB;

        physics::physicsComponent* physicsCompA;
        physics::physicsComponent* physicsCompB;

        physics::rigidbody* rigidbodyA;
        physics::rigidbody* rigidbodyB;

        std::unique_ptr<PenetrationQuery> penetrationInformation;

        bool isColliding;


    };
}
