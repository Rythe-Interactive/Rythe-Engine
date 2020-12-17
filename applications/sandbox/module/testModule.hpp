#pragma once
#include <core/core.hpp>
#include <core/math/math.hpp>
#include "../systems/testsystem.hpp"
#include "../systems/testsystem2.hpp"
#include "../systems/testsystemconvexhull.hpp"
#include "../systems/simplecameracontroller.hpp"



using namespace legion;

class TestModule : public Module
{
public:
    virtual void setup() override
    {
        reportSystem<TestSystemConvexHull>();
        reportSystem<SimpleCameraController>();
    }

    virtual priority_type priority() override
    {
        return default_priority;
    }
};

