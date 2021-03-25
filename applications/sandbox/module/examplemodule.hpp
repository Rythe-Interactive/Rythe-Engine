#pragma once
#include <core/core.hpp>

#include "../systems/examplesystem.hpp"

class ExampleModule : public legion::Module
{
public:
    virtual void setup() override
    {
        lgn::log::debug("ExampleModule setup");
        reportSystem<ExampleSystem>();
    }
};
