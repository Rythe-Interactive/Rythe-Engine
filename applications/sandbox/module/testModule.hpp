#pragma once
#include <core/core.hpp>
#include <core/math/math.hpp>
#include "../systems/testsystem.hpp"
#include "../systems/testsystem2.hpp"
#include "../systems/pointcloudtestsystem2.hpp"
#include "../systems/simplecameracontroller.hpp"
#include "../systems/gui_test.hpp"

#include <rendering/systems/pointcloudgeneration.hpp>


using namespace legion;

class TestModule : public Module
{
public:
    virtual void setup() override
    {
        reportSystem<SimpleCameraController>();

        //addProcessChain("TestChain");
        //reportComponentType<sah>();
        //reportSystem<pointcloudtestsystem2>();
        //reportSystem<PointCloudGeneration>();
        //reportSystem<TestSystem>();
        //reportSystem<TestSystem2>();
        reportSystem<GuiTestSystem>();
    }

    virtual priority_type priority() override
    {
        return default_priority;
    }
};

