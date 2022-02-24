#pragma once
#include <core/core.hpp>
#include <application/application.hpp>
#include <rendering/rendering.hpp>
#include <audio/audio.hpp>

struct example_comp
{

};

class ExampleSystem final : public legion::System<ExampleSystem>
{
    lgn::size_type frames = 0;
    lgn::time64 totalTime = 0;
    lgn::time::stopwatch<lgn::time64> timer;
    std::array<lgn::time64, 18000> times;

public:
    void setup()
    {
        using namespace legion;
        log::filter(log::severity_debug);
        log::debug("ExampleSystem setup");

        app::window& win = ecs::world.get_component<app::window>();
        app::context_guard guard(win);

        auto model = gfx::ModelCache::create_model("Sphere", fs::view("assets://models/sphere.obj"));

        auto material = gfx::MaterialCache::create_material("Texture", fs::view("assets://shaders/texture.shs"));
        material.set_param("_texture", gfx::TextureCache::create_texture(fs::view("engine://resources/default/albedo")));
        {
            auto ent = createEntity("Sun");
            ent.add_component(gfx::light::directional(math::color(1, 1, 0.8f), 10.f));
            auto [pos, rot, scal] = ent.add_component<transform>();
            rot = rotation::lookat(math::float3::zero, math::float3(-1, -1, -1));
        }

#if defined(LEGION_DEBUG)
        for (int i = 0; i < 2000; i++)
#else
        for (int i = 0; i < 20000; i++)
#endif
        {
            auto ent = createEntity();

            auto [pos, rot, scal] = ent.add_component<transform>();
            pos = math::sphericalRand(5.f);
            scal = scale(0.1f, 0.1f, 0.1f);

            ent.add_component<example_comp, velocity>();
            ent.add_component(gfx::mesh_renderer(material, model));
        }

        {
            model = gfx::ModelCache::create_model("Cube", fs::view("assets://models/Cube.obj"));
            auto ent = createEntity();
            auto [pos, rot, scal] = ent.add_component<transform>();
            pos->z = 30.f;
            scal = scale(2.f);
            ent.add_component(gfx::mesh_renderer(material, model));
        }

        model = gfx::ModelCache::create_model("Sponza", fs::view("assets://models/Sponza/Sponza.gltf"));

        material = gfx::MaterialCache::create_material("Test", fs::view("assets://shaders/uv.shs"));

        auto ent = createEntity();
        ent.add_component<transform>();
        ent.add_component(gfx::mesh_renderer(material, model));

        model = gfx::ModelCache::create_model("Fire place", fs::view("assets://models/fireplace.glb"));

        ent = createEntity();
        auto [pos, rot, scal] = ent.add_component<transform>();
        pos->x = 30.f;
        scal = scale(2.f);
        ent.add_component(gfx::mesh_renderer(material, model));


        bindToEvent<events::exit, &ExampleSystem::onExit>();
    }

    void shutdown()
    {
        lgn::log::debug("ExampleSystem shutdown");
    }

    void onExit(L_MAYBEUNUSED lgn::events::exit& event)
    {
        using namespace legion;

        time64 avg0 = totalTime / frames;
        time64 avg1 = timer.elapsed_time() / frames;

        std::set<time64, std::greater<time64>> orderedTimes;

        for (auto& time : times)
            orderedTimes.insert(time);

        time64 onePcLow = 0;
        time64 pointOnePcLow = 0;

        size_type i = 0;
        for (auto& time : orderedTimes)
        {
            i++;
            onePcLow += time;

            if (i <= math::max<size_type>(math::uround(frames / 1000.0), 1))
            {
                pointOnePcLow += time;
            }

            if (i >= math::max<size_type>(math::uround(frames / 100.0), 1))
            {
                break;
            }
        }

        pointOnePcLow /= math::max<size_type>(math::uround(frames / 1000.0), 1);
        onePcLow /= math::max<size_type>(math::uround(frames / 100.0), 1);

        log::info("1%Low {:.3f} 0.1%Low {:.3f} Avg {:.3f} Measured Avg {:.3f}", onePcLow, pointOnePcLow, avg0, avg1);
        log::info("1%Low {:.3f} 0.1%Low {:.3f} Avg {:.3f} Measured Avg {:.3f}", 1.0 / onePcLow, 1.0 / pointOnePcLow, 1.0 / avg0, 1.0 / avg1);
    }

    void update(legion::time::span deltaTime)
    {
        using namespace legion;
        static bool firstFrame = true;
        if (firstFrame)
        {
            timer.start();
            firstFrame = false;
        }

        ecs::filter<position, velocity, example_comp> filter;

        float dt = deltaTime;
        if (dt > 0.07f)
            return;

        if (filter.size())
        {
            auto poolSize = (schd::Scheduler::jobPoolSize() + 1);
            size_type jobSize = math::iround(math::ceil(filter.size() / static_cast<float>(poolSize)));

            queueJobs(poolSize, [&](id_type jobId)
                {
                    auto start = jobId * jobSize;
                    auto end = start + jobSize;
                    if (end > filter.size())
                        end = filter.size();

                    for (size_type i = start; i < end; i++)
                    {
                        auto& pos = filter[i].get_component<position>().get();
                        auto& vel = filter[i].get_component<velocity>().get();

                        if (vel == math::float3::zero)
                            vel = math::normalize(pos);

                        math::float3 perp;

                        perp = math::normalize(math::cross(vel, math::float3::up));

                        math::float3 rotated = (math::axisAngleMatrix(vel, math::perlin(pos) * math::pi<float>()) * math::float4(perp.x, perp.y, perp.z, 0)).xyz();
                        rotated.y -= 0.5f;
                        rotated = math::normalize(rotated);

                        vel = math::normalize(vel + rotated * dt);

                        if (math::abs(vel.y) >= 0.9f)
                        {
                            auto rand = math::circularRand(1.f);
                            vel.y = 0.9f;
                            vel = math::normalize(vel + math::float3(rand.x, 0.f, rand.y));
                        }

                        pos += vel * 0.3f * dt;
                    }
                }
            ).wait();
        }

        time64 delta = schd::Clock::lastTickDuration();

        if (frames < times.size())
        {
            times[frames] = delta;
            frames++;
            totalTime += delta;
        }
        else
        {

            raiseEvent<events::exit>();
        }
    }
};
