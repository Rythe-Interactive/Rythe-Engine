#pragma once
#include <core/core.hpp>
#include <core/math/math.hpp>
#include "../systems/testsystem.hpp"
#include"../systems/PointCloudTestSystem.hpp"
#include "../systems/simplecameracontroller.hpp"



using namespace legion;

class TestModule : public Module
{
public:
    virtual void setup() override
    {

        if (true)
        {
            reportSystem<PointCloudTestSystem>();
        }
        else
        {
            addProcessChain("TestChain");
            reportComponentType<sah>();
            reportSystem<TestSystem>();
        }
        //reportSystem<TestSystem2>();
        reportSystem<SimpleCameraController>();
    }

    virtual priority_type priority() override
    {
        return default_priority;
    }
};

