#pragma once
#include <core/core.hpp>
#include <iostream>

using namespace legion;

struct sah
{
    int value;
};

//using namespace legion::core::filesystem::literals;

class TestSystem final : public System<TestSystem>
{
public:
    virtual void setup()
    {
        std::cout << "TestSystem\n";
        //createProcess<&TestSystem::update>("Update");
    }

    void update(time::span deltaTime)
    {
        
    }
};
