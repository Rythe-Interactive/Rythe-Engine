#pragma once
#include <core/core.hpp>
#include "../systems/testsystem.hpp"

using namespace legion;

class TestModule : public Module
{
public:
    virtual void setup() override
    {
        std::cout << "TestModule\n";

        // Create the request for the main window.
        //app::WindowSystem::requestWindow(
        //    world_entity_id,
        //    math::ivec2(1920, 1080),
        //    "LEGION Engine",
        //    "Legion Icon",
        //    nullptr,
        //    nullptr,
        //    1
        //);

       // reportComponentType<sah>();
        reportSystem<TestSystem>();
    }

    virtual priority_type priority() override
    {
        return default_priority;
    }
};

