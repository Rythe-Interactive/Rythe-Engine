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
        using namespace legion;
        log::filter(log::severity_debug);
        log::debug("ExampleSystem setup");

        for (int i = 0; i < 20000; i++)
            createEntity().add_component<example_component>();
    }
        
    void update(legion::time::span deltaTime)
    {
        using namespace legion;
        ecs::filter<example_component> filter;

        static std::atomic<size_type> sum = { 0 };

        queueJobs(filter.size(), [&](id_type jobID) {
            auto comp = filter[jobID].get_component<example_component>();
            sum.fetch_add(comp->value++, std::memory_order_relaxed);
            }).wait();

        static fast_time buffer = 0;
        static fast_time avgTime = deltaTime;
        avgTime = (avgTime + deltaTime) / 2.f;
        buffer += deltaTime;

        if (buffer >= 1.f)
        {
            buffer--;
            log::debug("sum: {} dt: {}s fps: {}", sum.load(std::memory_order_relaxed), avgTime, 1.f / avgTime);
        }
    }
};
