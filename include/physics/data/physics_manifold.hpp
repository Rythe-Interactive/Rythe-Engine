#pragma once

#include <physics/colliders/physicscollider.hpp>
#include <core/core.hpp>
#include <physics/physics_contact.hpp>
#include <core/core.hpp>
#include <physics/components/physics_component.hpp>


namespace legion::physics
{

    /** @struct physics_manifold_precursor
    * @brief contains the necessary information to detect and resolve a collision
    */

    class PhysicsCollider;
    class PenetrationQuery;

    struct physics_manifold
    {
        std::vector<physics_contact> contacts;

        math::mat4 transformA;
        math::mat4 transformB;

        std::shared_ptr<PhysicsCollider> colliderA;
        std::shared_ptr<PhysicsCollider> colliderB;

        ecs::component_handle<physics::physicsComponent> physicsCompA;
        ecs::component_handle<physics::physicsComponent> physicsCompB;

        std::shared_ptr<PenetrationQuery> penetrationInformation;

        bool isColliding;


    };
}
