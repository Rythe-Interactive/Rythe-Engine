#pragma once

#include <physics/colliders/physicscollider.hpp>
#include <core/core.hpp>
#include <physics/physics_contact.hpp>
#include <core/core.hpp>
#include <physics/components/physics_component.hpp>


namespace legion::physics
{
    class PhysicsCollider;
    class PenetrationQuery;

    /** @struct physics_manifold
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

        std::shared_ptr<PenetrationQuery> penetrationInformation;

        bool isColliding;

        void DEBUG_checkIDAndBreak(std::string firstID,std::string secondID) const
        {
            auto idHA = physicsCompA.entity.get_component_handle<identifier>();
            auto idHB = physicsCompB.entity.get_component_handle<identifier>();

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
        }

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
            auto idHA = physicsCompA.entity.get_component_handle<identifier>();
            auto idHB = physicsCompB.entity.get_component_handle<identifier>();

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
