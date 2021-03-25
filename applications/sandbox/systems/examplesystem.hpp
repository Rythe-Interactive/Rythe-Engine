#pragma once
#include <core/core.hpp>

struct example_component
{
    int value = 1;
};

class ExampleSystem final : public legion::System<ExampleSystem>
{
public:
    virtual void setup()
    {
        lgn::log::filter(lgn::log::severity_debug);
        lgn::log::debug("ExampleSystem setup");

        auto ent = lgn::ecs::Registry::createEntity();
        ent.add_component<example_component>();
    }

    void update(legion::time::span deltaTime)
    {
        lgn::ecs::filter<example_component> filter;

        int sum = 0;
        for (auto& ent : filter)
        {
            sum += ent.get_component<example_component>()->value;
        }
        lgn::log::debug("sum: {} dt: {} fps: {}", sum, deltaTime.seconds(), 1.f / deltaTime.seconds());
    }
};
