#pragma once
#include <core/core.hpp>
#include <application/application.hpp>
#include <rendering/rendering.hpp>

class ExampleSystem final : public legion::System<ExampleSystem>
{
public:
    virtual void setup()
    {
        using namespace legion;
        log::filter(log::severity_debug);
        log::debug("ExampleSystem setup");

        schd::Scheduler::reserveThread([]() {
            log::info("Thread {} assigned.", std::this_thread::get_id());
            });

        app::window& win = ecs::world.get_component<app::window>();
        app::context_guard guard(win);

        auto model = gfx::ModelCache::create_model("Cube", fs::view("assets://models/Cube.obj"));
        auto material = gfx::MaterialCache::create_material("Default", fs::view("assets://shaders/texture.shs"));
        material.set_param("_texture", gfx::TextureCache::create_texture(fs::view("engine://resources/default/albedo")));

        for (int i = 0; i < 20000; i++)
        {
            auto ent = createEntity();
            ent.add_component<transform>();
            ent.add_component(gfx::mesh_renderer(material, model));
        }
    }
        
    void update(legion::time::span deltaTime)
    {
        using namespace legion;
        ecs::filter<position> filter;

        queueJobs(filter.size(), [&](id_type jobID) {
            auto& pos = filter[jobID].get_component<position>().get();
            pos += math::sphericalRand(1.f);
            }).wait();

        static fast_time buffer = 0;
        static fast_time avgTime = deltaTime;
        avgTime = (avgTime + deltaTime) / 2.f;
        buffer += deltaTime;

        if (buffer >= 1.f)
        {
            buffer--;
            log::debug("dt: {}s fps: {}", avgTime, 1.f / avgTime);
        }
    }
};
