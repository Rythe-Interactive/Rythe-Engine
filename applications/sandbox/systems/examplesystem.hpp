#pragma once
#include <core/core.hpp>
#include <application/application.hpp>
#include <rendering/rendering.hpp>

struct example_comp
{

};

class ExampleSystem final : public legion::System<ExampleSystem>
{
public:
    void setup()
    {
        using namespace legion;
        log::filter(log::severity_debug);
        log::debug("ExampleSystem setup");

        (void)schd::Scheduler::reserveThread([]() {
            log::info("Thread {} assigned.", std::this_thread::get_id());
            });

        app::window& win = ecs::world.get_component<app::window>();
        app::context_guard guard(win);

        auto model = gfx::ModelCache::create_model("Sphere", fs::view("assets://models/sphere.obj"));

        //auto material = gfx::MaterialCache::create_material("Default", fs::view("assets://shaders/pbr.shs"));
        //material.set_param(SV_ALBEDO, gfx::TextureCache::create_texture(fs::view("engine://resources/default/albedo")));
        //material.set_param(SV_NORMALHEIGHT, gfx::TextureCache::create_texture(fs::view("engine://resources/default/normalHeight")));
        //material.set_param(SV_MRDAO, gfx::TextureCache::create_texture(fs::view("engine://resources/default/MRDAo")));
        //material.set_param(SV_EMISSIVE, gfx::TextureCache::create_texture(fs::view("engine://resources/default/emissive")));
        //material.set_param(SV_HEIGHTSCALE, 1.f);
        //material.set_param("discardExcess", false);
        //material.set_param("skycolor", math::color(0.1f, 0.3f, 1.0f));

        auto material = gfx::MaterialCache::create_material("Default", fs::view("assets://shaders/texture.shs"));
        material.set_param("_texture", gfx::TextureCache::create_texture(fs::view("engine://resources/default/albedo")));

        {
            auto ent = createEntity("Sun");
            ent.add_component(gfx::light::directional());
            auto [pos, rot, scal] = ent.add_component<transform>();
            rot = rotation::lookat(math::vec3::zero, math::vec3(-1, -1, -1));
        }

        for (int i = 0; i < 20000; i++)
        {
            auto ent = createEntity();

            auto [pos, rot, scal] = ent.add_component<transform>();
            pos = math::sphericalRand(5.f);
            scal = scale(0.1f, 0.1f, 0.1f);

            ent.add_component<example_comp, velocity>();
            ent.add_component(gfx::mesh_renderer(material, model));
        }
    }

    void update(legion::time::span deltaTime)
    {
        using namespace legion;
        ecs::filter<position, velocity, example_comp> filter;

        float dt = deltaTime;
        if (dt > 0.07f)
            return;

        queueJobs(filter.size(), [&](id_type jobId)
            //for (size_type jobId = 0; jobId < filter.size(); jobId++)
            {
                auto& pos = filter[jobId].get_component<position>().get();
                auto& vel = filter[jobId].get_component<velocity>().get();

                if (vel == math::vec3::zero)
                    vel = math::normalize(pos);

                math::vec3 perp;                

                perp = math::normalize(math::cross(vel, math::vec3::up));

                math::vec3 rotated = (math::axisAngleMatrix(vel, math::perlin(pos) * math::pi<float>()) * math::vec4(perp.x, perp.y, perp.z, 0)).xyz();
                rotated.y -= 0.5f;
                rotated = math::normalize(rotated);

                vel = math::normalize(vel + rotated * dt);

                if (math::abs(vel.y) >= 0.9f)
                {
                    auto rand = math::circularRand(1.f);
                    vel.y = 0.9f;
                    vel = math::normalize(vel + math::vec3(rand.x, 0.f, rand.y));
                }

                pos += vel * 0.3f * dt;
            }
        ).wait();

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
