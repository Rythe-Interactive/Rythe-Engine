#pragma once

#include <core/core.hpp>
#include <physics/systems/physicssystem.hpp>
#include <physics/components/physics_component.hpp>
#include <physics/components/rigidbody.hpp>
#include <physics/components/fracturecountdown.hpp>

namespace legion::physics
{
    class PhysicsModule : public Module
    {

    public:

        virtual void setup() override
        {
            log::debug("physics is setup!");

           /* reportComponentType<physicsComponent>();
            reportComponentType<rigidbody>();*/

            /*createProcessChain("Physics");
            reportSystem<PhysicsSystem>();
            
            reportComponentType<identifier>();
            reportComponentType<MeshSplitter>();
            reportComponentType<Fracturer>();
            reportComponentType<FractureCountdown>();
            reportComponentType<ObjectToFollow>();*/
            //reportComponentType <addRB>();
        }

        virtual priority_type priority() override
        {
            return 20;
        }

    };

}


