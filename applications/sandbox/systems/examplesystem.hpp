#pragma once
#include <core/core.hpp>

struct example_component
{
    int value = 1;
};

struct example_event : public legion::events::event<example_event>
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

        for (int i = 0; i < 1000; i++)
            createEntity().add_component<example_component>();

        bindToEvent<example_event, &ExampleSystem::onExampleEvent>();
    }

    void onExampleEvent(example_event& event)
    {
        using namespace legion;
        log::debug("event id: {} value: {}", event.id, event.value);
    }

    void update(legion::time::span deltaTime)
    {
        raiseEvent<example_event>();

        using namespace legion;
        ecs::filter<example_component> filter;

        std::atomic_int sum;

        auto comps = filter.get<example_component>();
        queueJobs(filter.size(), [&]()
            {            
                sum.fetch_add(comps[async::this_job::get_id()].get().value, std::memory_order_relaxed);
            }).wait();

        static fast_time buffer = 0;
        static fast_time totalTime = 0;
        static size_type frameCount = 0;
        frameCount++;
        totalTime += deltaTime;
        buffer += deltaTime;

        if (buffer >= 1.f)
        {
            buffer--;
            auto dt = totalTime / frameCount;
            log::debug("sum: {} dt: {}s fps: {}", sum.load(std::memory_order_relaxed), dt, 1.f / dt);
        }
    }
};
