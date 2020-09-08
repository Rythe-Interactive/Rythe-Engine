#pragma once

#include <core/core.hpp>
#include <physics/PhysicsSystem.hpp>

namespace args::physics
{
    class PhysicsModule : public Module
    {
    public:

        virtual void setup() override
        {
            addProcessChain("Physics");
            reportSystem<PhysicsSystem>();
        }

        virtual priority_type priority() override
        {
            return 20;
        }

    };

}


