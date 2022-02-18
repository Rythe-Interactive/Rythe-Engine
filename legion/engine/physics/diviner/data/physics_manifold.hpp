#pragma once

#include <physics/diviner/colliders/physicscollider.hpp>
#include <core/core.hpp>
#include <physics/diviner/physics_contact.hpp>
#include <core/core.hpp>
#include <physics/diviner/components/physics_component.hpp>
#include <physics/diviner/data/penetrationquery.hpp>

namespace legion::physics
{
    class PhysicsCollider;

    /** @struct physics_manifold
    * @brief contains the necessary information to detect and resolve a collision.
    * Only exist within one physics timestep.
    */
    struct physics_manifold
    {
        std::vector<physics_contact> contacts;

        math::mat4 transformA;
        math::mat4 transformB;

        PhysicsCollider* colliderA = nullptr;
        PhysicsCollider* colliderB = nullptr;

        ecs::entity entityA;
        ecs::entity entityB;

        physics::physicsComponent* physicsCompA = nullptr;
        physics::physicsComponent* physicsCompB = nullptr;

        physics::rigidbody* rigidbodyA = nullptr;
        physics::rigidbody* rigidbodyB = nullptr;

        std::unique_ptr<PenetrationQuery> penetrationInformation;

        bool isColliding;

        bool DEBUG_checkID(std::string firstID, std::string secondID) const
        {
            std::string str1, str2;
            GetPairID(str1, str2);

            if ((str1 == firstID && str2 == secondID)
                || (str2 == firstID && str1 == secondID))
            {
                return true;
            }
            else
            {
                return false;
            }
        }

        void GetPairID(std::string& str1, std::string& str2) const
        {
            auto idHA = entityA.get_component<identifier>();
            auto idHB = entityB.get_component<identifier>();

            str1 = "" ;
            str2 = "" ;

            if (idHA)
            {
                str1 = idHA->id;
            }

            if (idHB)
            {
                str2 = idHB->id;
            }

        }
    };
}
