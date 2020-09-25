#pragma once
#include <physics/physicsimport.h>
#include <physics/physics_contact.h>
#include <physics/physicsimport.h>
#include <physics/physics_component.hpp>

namespace args::physics
{
    /** @struct physics_manifold_precursor
    * @brief contains the necessary information to detect and resolve a collision
    */
    struct physics_manifold
    {
        std::vector<physics_contact> contacts;

        math::mat4 transformA;
        math::mat4 transformB;

        std::shared_ptr<PhysicsCollider> colliderA;
        std::shared_ptr<PhysicsCollider> colliderB;

        ecs::component_handle<physics::physicsComponent> physicsCompA;
        ecs::component_handle<physics::physicsComponent> physicsCompB;

        bool isColliding;


    };
}
