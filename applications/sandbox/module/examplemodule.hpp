#pragma once
#include <core/core.hpp>

#include "../systems/mysystem.hpp"
#include "../systems/examplesystem.hpp"
#include "../systems/simplecameracontroller.hpp"
#include "../systems/gui_test.hpp"

class ExampleModule : public rythe::Module
{
public:
    virtual void setup() override
    {
        using namespace rythe;
        app::WindowSystem::requestWindow(
            ecs::world_entity_id,
            math::ivec2(1920, 1080),
            "RYTHE Engine",
            "RYTHE Icon",
            nullptr,
            nullptr,
            0
        );

        //reportSystem<MySystem>();
        reportSystem<SimpleCameraController>();
        reportSystem<ExampleSystem>();
        reportSystem<GuiTestSystem>();
    }
};
