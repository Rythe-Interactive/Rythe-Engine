#pragma once
#include <core/core.hpp>
#include <core/math/math.hpp>
#include "../systems/testsystem.hpp"

#include "../systems/testsystemconvexhull.hpp"
//#include "../systems/testsystem2.hpp"
//#include"../systems/pointcloudtestsystem2.hpp"
#include "../systems/simplecameracontroller.hpp"
#include "../systems/gui_test.hpp"
#include "../systems/testsystem2.hpp"
//#include"../systems/pointcloudtestsystem2.hpp"
#include "../systems/simplecameracontroller.hpp"
#include "../systems/scenetestsystem1.hpp"
#include <rendering/systems/pointcloudgeneration.hpp>

#include <physics/systems/physics_fracture_test_system.hpp>


#include "../data/animation.hpp"
#include "../systems/animation_editor.hpp"
#include "../systems/animator.hpp"



using namespace legion;

class TestModule : public Module
{
public:
    virtual void setup() override
    {
        //reportSystem<TestSystem>();
        //reportSystem<TestSystemConvexHull>();
        app::WindowSystem::requestWindow(world_entity_id, math::ivec2(1920, 1080), "LEGION Engine", "Legion Icon", nullptr, nullptr, 1); // Create the request for the main window.
        reportComponentType<sah>();
        reportComponentType<ext::animation>();
        reportSystem<ext::Animator>();
        reportSystem<ext::AnimationEditor>();
        reportSystem<TestSystem>();
        reportSystem<SceneTestSystem1>();
        reportSystem<SimpleCameraController>();
        reportSystem<GuiTestSystem>();
        reportSystem<physics::PhysicsFractureTestSystem>();

    }

    virtual priority_type priority() override
    {
        return default_priority;
    }
};

