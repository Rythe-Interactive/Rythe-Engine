#pragma once
#include <core/core.hpp>
#include <core/math/math.hpp>
#include "../systems/testsystem.hpp"
#include "../systems/testsystem2.hpp"
#include"../systems/pointcloudtestsystem2.hpp"
#include "../systems/simplecameracontroller.hpp"

#include<rendering/systems/pointcloudgeneration.hpp>

using namespace legion;

class TestModule : public Module
{
public:
    virtual void setup() override
    {

        //if (false)
        //{
        //    reportSystem<pointcloudtestsystem2>();
        //    //    reportSystem<PointCloudGeneration>();
        //}
        //else
        //{
        //    addProcessChain("TestChain");
        //    reportComponentType<sah>();
        //    reportSystem<TestSystem>();
        //}
        reportSystem<TestSystem2>();
        reportSystem<SimpleCameraController>();
    }

    virtual priority_type priority() override
    {
        return default_priority;
    }
};

