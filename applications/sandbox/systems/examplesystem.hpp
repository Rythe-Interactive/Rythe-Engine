#pragma once
#include <core/core.hpp>
#include <application/application.hpp>
#include <rendering/rendering.hpp>
#include <audio/audio.hpp>

#include <core/serialization/serializationregistry.hpp>
#include <core/ecs/handles/entity.hpp>

using json = nlohmann::ordered_json;

namespace legion::core
{
    struct example_comp
    {
        int value = 1;
    };

    struct scene_comp
    {
        id_type id = -1;
        std::vector<ecs::entity> entities;
    };
}


ManualReflector(example_comp, value);
ManualReflector(scene_comp, id, entities);


class ExampleSystem final : public legion::System<ExampleSystem>
{
public:
    void setup()
    {
        using namespace legion;
        log::filter(log::severity_debug);
        log::debug("ExampleSystem setup");

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

        auto audioSegment = audio::AudioSegmentCache::createAudioSegment("Beep", fs::view("assets://audio/beep4.mp3"));

        {
            auto ent = createEntity("Sun");
            ent.add_component(gfx::light::directional(math::color(1, 1, 0.8f), 10.f));
            auto [pos, rot, scal] = ent.add_component<transform>();
            rot = rotation::lookat(math::vec3::zero, math::vec3(-1, -1, -1));
        }



        //Serialization Test
        std::string_view filePath = "assets://scenes/mainscene.json";

        serialization::serializer_registry::register_serializer<example_comp>();
        serialization::serializer_registry::register_serializer<position>();
        //serialization::serializer_registry::register_serializer<scale>();
        //serialization::serializer_registry::register_serializer<rotation>();
        serialization::serializer_registry::register_serializer<velocity>();
        serialization::serializer_registry::register_serializer<mesh_filter>();
        //serialization::serializer_registry::register_serializer<int>();
        //serialization::serializer_registry::register_serializer<float>();
        serialization::serializer_registry::register_serializer<assets::asset<mesh>>();
        //serialization::serializer_registry::register_serializer<transform>();
        serialization::serializer_registry::register_serializer<rendering::mesh_renderer>();
        auto serializer = serialization::serializer_registry::get_serializer<scene_comp>();
        auto scene = scene_comp();
        scene.id = 1;


        mesh_filter meshFilter;
        meshFilter.shared_mesh = model.get_mesh();

        for (int i = 0; i < 20; i++)
        {
            auto ent = createEntity();
            auto child = createEntity();
            ent.add_child(child);
            ent.add_component<example_comp>();
            ent.add_component<mesh_filter>(meshFilter);
            ent.add_component<rendering::mesh_renderer>(rendering::mesh_renderer(material));
            ent.add_component<position>();
            ent.add_component<velocity>();
            scene.entities.push_back(ent);
        }

        serializer->write(&scene, "Main", fs::view(filePath));
    }

    void update(legion::time::span deltaTime)
    {
        using namespace legion;

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
                }
            ).wait();
        }

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
