#pragma once

///
/// This system is not used in the project and just here for an example!!!
///

#include <core/core.hpp>
#include <physics/physics.hpp>

using namespace legion;
using namespace legion::time::literals;
using namespace legion::physics;

struct my_component { int myVal; };

class MySystem final : public System<MySystem>
{
public:
    void setup()
    {
        bindToEvent<events::exit, &MySystem::onExit>();
        createProcess<&MySystem::physicsUpdate>("Physics", 20_ms);
    }

    void onExit(events::exit& event)
    {
        log::debug("Exiting with code: {}", event.exitcode);
    }

    void update(time::span deltaTime)
    {
        static ecs::filter<my_component, position> filter;

        for (auto ent : filter)
        {
            ent.get_component<my_component>()->myVal++;
            position& pos = ent.get_component<position>();
            pos.x++;
        }
    }

    void physicsUpdate(time::span deltaTime)
    {
        static ecs::filter<my_component, rigidbody> filter;

        queueJobs(filter.size(), [&](id_type jobId)
            {
                auto handle = filter[jobId].get_component<velocity>();
                handle->x--;
            }
        ).wait();
    }
};





