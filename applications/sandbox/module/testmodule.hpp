#pragma once
#include "../systems/gui_test.hpp"

#include <core/core.hpp>
#include <core/math/math.hpp>
#include "../systems/testsystem.hpp"
#include "../systems/simplecameracontroller.hpp"


using namespace legion;

class TestModule : public Module
{
public:
    virtual void setup() override
    {
        // Create the request for the main window.
        app::WindowSystem::requestWindow(
            world_entity_id,
            math::ivec2(1920, 1080),
            "LEGION Engine",
            "Legion Icon",
            nullptr,
            nullptr,
            1
        );

        reportComponentType<custom_component>();
        reportSystem<SimpleCameraController>();
        reportSystem<GuiTestSystem>();
        reportSystem<CustomSystem>();
    }

    virtual priority_type priority() override
    {
        return default_priority;
    }
};

