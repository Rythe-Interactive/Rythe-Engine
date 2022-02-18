#pragma once
#include <core/core.hpp>
#include <physics/diviner/systems/physicssystem.hpp>
#include <physics/diviner/components/physics_component.hpp>
#include <physics/diviner/components/rigidbody.hpp>

namespace legion::physics
{
    class PhysicsModule : public Module
    {
    public:

        virtual void setup() override
        {
            createProcessChain("Physics");
            reportSystem<PhysicsSystem>();
            registerComponentType<physicsComponent>();
            registerComponentType<rigidbody>();
            registerComponentType<identifier>();
        }

        virtual priority_type priority() override
        {
            return 20;
        }
    };
}
