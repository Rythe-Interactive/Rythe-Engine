#pragma once
#include <core/core.hpp>

#include "../systems/examplesystem.hpp"

class ExampleModule : public legion::Module
{
public:
    virtual void setup() override
    {
        using namespace legion;
        app::WindowSystem::requestWindow(
            ecs::world_entity_id,
            math::ivec2(1920, 1080),
            "LEGION Engine",
            "Legion Icon",
            nullptr,
            nullptr,
            1
        );

        lgn::log::debug("ExampleModule setup");
        reportSystem<ExampleSystem>();
    }
};
