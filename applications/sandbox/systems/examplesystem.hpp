#pragma once
#include <core/core.hpp>

#include <core/serialization/serializationregistry.hpp>
#include <core/ecs/handles/entity.hpp>


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

        serialization::SerializationRegistry::register_component<example_component>();
        example_component test;
        test.value = 10;
        auto j = serialization::json_serializer::serialize<example_component>(test);
        auto output = serialization::SerializationRegistry::getPrototype(j);
    }

    void update(legion::time::span deltaTime)
    {
        using namespace legion;
        ecs::filter<example_component> filter;

        static size_type sum = 0;

        for (auto& ent : filter)
            sum += ent.get_component<example_component>()->value;

        static fast_time buffer = 0;
        static fast_time avgTime = deltaTime;
        avgTime = (avgTime + deltaTime) / 2.f;
        buffer += deltaTime;

        if (buffer >= 1.f)
        {
            buffer--;
            log::debug("sum: {} dt: {}s fps: {}", sum, avgTime, 1.f / avgTime);
        }
    }
};
