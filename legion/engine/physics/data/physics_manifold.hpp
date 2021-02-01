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

        /*void DEBUG_checkIDAndBreak(std::string firstID,std::string secondID) const
        {
            auto idHA = entityA.get_component_handle<identifier>();
            auto idHB = entityB.get_component_handle<identifier>();

            if (idHA && idHB)
            {
                auto str1 = idHA.read().id;
                auto str2 = idHB.read().id;

                if ((str1 == firstID && str2 == secondID)
                    || (str2 == firstID && str1 == secondID))
                {
                    DebugBreak();
                }
            }
        }*/

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
            auto idHA = entityA.get_component_handle<identifier>();
            auto idHB = entityB.get_component_handle<identifier>();

            str1 = "" ;
            str2 = "" ;

            if (idHA)
            {
                str1 = idHA.read().id;
            }

            if (idHB)
            {
                str2 = idHB.read().id;
            }

        }

    };
}
