#pragma once
#include <core/core.hpp>
#include <application/application.hpp>
#include <rendering/rendering.hpp>

struct exit_action : public lgn::app::input_action<exit_action>{};

class ExampleSystem final : public legion::System<ExampleSystem>
{
public:
    virtual void setup()
    {
        using namespace legion;
        log::filter(log::severity_debug);
        log::debug("ExampleSystem setup");

        log::debug("_texture {} {} {}", nameHash("_texture"), nameHash(std::string("_texture")), nameHash(std::string("_texture\0")));

        app::InputSystem::createBinding<exit_action>(app::inputmap::method::ESCAPE);

        bindToEvent<exit_action, &ExampleSystem::onExit>();

        auto cament = createEntity();
        cament.add_component<gfx::camera>()->set_projection(60.f, 0.01f, 1000.f);
        cament.add_component(position(), rotation::lookat(math::vec3::zero, math::vec3::forward), scale());

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

    void onExit(exit_action& event)
    {
        raiseEvent<lgn::events::exit>();
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
