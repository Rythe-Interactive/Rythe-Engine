#pragma once

#include <core/core.hpp>
#include <physics/physicssystem.hpp>
#include <physics/physics_component.hpp>
#include <physics/rigidbody.hpp>

namespace args::physics
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

        }

        virtual priority_type priority() override
        {
            return 20;
        }

    };

}


