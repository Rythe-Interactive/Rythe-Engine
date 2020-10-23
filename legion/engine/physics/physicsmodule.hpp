#pragma once

#include <core/core.hpp>
#include <physics/systems/physicssystem.hpp>
#include <physics/components/physics_component.hpp>
#include <physics/components/rigidbody.hpp>

namespace legion::physics
{
    class PhysicsModule : public Module
    {
    public:

        virtual void setup() override
        {
            addProcessChain("Physics");
            reportSystem<PhysicsSystem>();
            reportComponentType<physicsComponent>();
            reportComponentType<rigidbody>();
            reportComponentType<identifier>();

        }

        virtual priority_type priority() override
        {
            return 20;
        }

    };

}


