#pragma once
#include <core/core.hpp>

struct example_component
{
    int value;
};

class TestSystem final : public legion::System<TestSystem>
{
public:
    virtual void setup()
    {
    }

    void update(legion::time::span deltaTime)
    {

    }
};
