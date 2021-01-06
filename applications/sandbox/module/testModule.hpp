#pragma once
#include <core/core.hpp>
#include <core/math/math.hpp>
#include "../systems/testsystem.hpp"

#include "../systems/testsystemconvexhull.hpp"
//#include "../systems/testsystem2.hpp"
#include"../systems/pointcloudtestsystem2.hpp"
#include "../systems/simplecameracontroller.hpp"
#include "../systems/gui_test.hpp"

#include <rendering/systems/pointcloudgeneration.hpp>


using namespace legion;

class TestModule : public Module
{
public:
    virtual void setup() override
    {
        app::WindowSystem::requestWindow(world_entity_id, math::ivec2(1920, 1080), "LEGION Engine", "Legion Icon", nullptr, nullptr, 1); // Create the request for the main window.
        reportComponentType<sah>();
      //  reportSystem<TestSystem>();
        reportSystem<SimpleCameraController>();
        reportSystem<GuiTestSystem>();
        reportSystem<pointcloudtestsystem2>();
    }

    virtual priority_type priority() override
    {
        return default_priority;
    }
};

