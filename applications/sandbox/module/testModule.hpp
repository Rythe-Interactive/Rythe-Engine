#pragma once
#include <core/core.hpp>
#include <core/math/math.hpp>
#include "../systems/testsystem.hpp"

#include "../systems/testsystemconvexhull.hpp"
//#include "../systems/testsystem2.hpp"
//#include"../systems/pointcloudtestsystem2.hpp"
#include "../systems/simplecameracontroller.hpp"
#include "../systems/gui_test.hpp"

#include <rendering/systems/pointcloudgeneration.hpp>
#include <physics/systems/physics_fracture_test_system.hpp>

using namespace legion;

class TestModule : public Module
{
public:
    virtual void setup() override
    {
        //reportSystem<TestSystem>();
        reportSystem<TestSystemConvexHull>();
        reportSystem<SimpleCameraController>();
        reportSystem<GuiTestSystem>();
        reportSystem<physics::PhysicsFractureTestSystem>();

    }

    virtual priority_type priority() override
    {
        return default_priority;
    }
};

