#pragma once
#include <core/core.hpp>

class ExampleModule : public legion::Module
{
public:
    virtual void setup() override
    {
        createProcessChain("Update");
    }
};
