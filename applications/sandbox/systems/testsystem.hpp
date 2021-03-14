#pragma once
#include <core/core.hpp>
#include <rendering/rendering.hpp>
#include "gui_test.hpp"

using namespace legion;

// A custom component can be any struct you need.
// DONT FORGET TO REPORT THE COMPONENT THOUGH!!!!
// In general you would also rather have smaller components but have more of them.
// That's also the reason we have position, rotation, and scale components instead of a transform component.
struct custom_component
{
    int value;
};

struct light_switch : public app::input_action<light_switch> {};
struct tonemap_switch : public app::input_action<tonemap_switch> {};

using namespace legion::core::filesystem::literals;
using namespace legion::core::scenemanagement;

class CustomSystem final : public System<CustomSystem>
{
public:
    ecs::entity_handle sun;
    std::vector<gfx::material_handle> skyColorMaterials;

    virtual void setup()
    {
        // Creates an update function on the "Update" thread.
        createProcess<&CustomSystem::update>("Update");

#pragma region Input binding

        // Little tip: Mouse over functions you see here to read the documentation supplied with it.
        //             Alternatively you can also ctrl+left click on the function or press F12 with the type cursor in the name.
        //             These actions would take you to where the functions are defined in the code.
        // (PS. I'm sorry though not everything is documented equally well... I'm working on it...)
        app::InputSystem::createBinding<light_switch>(app::inputmap::method::F);
        app::InputSystem::createBinding<tonemap_switch>(app::inputmap::method::G);

        bindToEvent<light_switch, &CustomSystem::onLightSwitch>();
        bindToEvent<tonemap_switch, &CustomSystem::onTonemapSwitch>();
#pragma endregion

#pragma region Model and material loading
        const float additionalLightIntensity = 0.5f;

        gfx::model_handle directionalLightH;
        gfx::model_handle spotLightH;
        gfx::model_handle pointLightH;
        gfx::model_handle cubeH;
        gfx::model_handle sphereH;
        gfx::model_handle suzanneH;
        gfx::model_handle uvsphereH;
        gfx::model_handle planeH;

        gfx::material_handle wireframeMH;
        gfx::material_handle vertexColorMH;
        gfx::material_handle uvMH;
        gfx::material_handle normalMH;

        gfx::material_handle directionalLightMH;
        gfx::material_handle spotLightMH;
        gfx::material_handle pointLightMH;

        gfx::material_handle defaultMH;
        gfx::material_handle copperMH;
        gfx::material_handle graniteMH;
        gfx::material_handle granite2MH;
        gfx::material_handle rockMH;
        gfx::material_handle rock2MH;

        std::vector<gfx::material_handle> suzanneMaterials;

        app::window window = m_ecs->world.get_component_handle<app::window>().read();

        {
            app::context_guard guard(window);

#pragma region Models
            directionalLightH = gfx::ModelCache::create_model("directional light", "assets://models/directional-light.obj"_view);
            spotLightH = gfx::ModelCache::create_model("spot light", "assets://models/spot-light.obj"_view);
            pointLightH = gfx::ModelCache::create_model("point light", "assets://models/point-light.obj"_view);
            cubeH = gfx::ModelCache::create_model("cube", "assets://models/cube.obj"_view);
            sphereH = gfx::ModelCache::create_model("sphere", "assets://models/sphere.obj"_view);
            suzanneH = gfx::ModelCache::create_model("suzanne", "assets://models/suzanne-test.obj"_view, suzanneMaterials);
            uvsphereH = gfx::ModelCache::create_model("uvsphere", "assets://models/uvsphere.obj"_view);
            planeH = gfx::ModelCache::create_model("plane", "assets://models/plane.obj"_view);
#pragma endregion

#pragma region Materials
            /////////////////////
            // Debug materials //
            /////////////////////
            wireframeMH = gfx::MaterialCache::create_material("wireframe", "assets://shaders/wireframe.shs"_view);
            vertexColorMH = gfx::MaterialCache::create_material("vertex color", "assets://shaders/vertexcolor.shs"_view);
            uvMH = gfx::MaterialCache::create_material("uv", "assets://shaders/uv.shs"_view);
            normalMH = gfx::MaterialCache::create_material("normal", "assets://shaders/normal.shs"_view);
            normalMH.set_param(SV_NORMALHEIGHT, gfx::TextureCache::create_texture("engine://resources/default/normalHeight"_view));

            /////////////////////
            // Light materials //
            /////////////////////
            auto lightshader = gfx::ShaderCache::create_shader("light", "assets://shaders/light.shs"_view);

            directionalLightMH = gfx::MaterialCache::create_material("directional light", lightshader);
            directionalLightMH.set_param("color", math::color(1, 1, 0.8f));
            directionalLightMH.set_param("intensity", 1.f);

            spotLightMH = gfx::MaterialCache::create_material("spot light", lightshader);
            spotLightMH.set_param("color", math::colors::green);
            spotLightMH.set_param("intensity", additionalLightIntensity);

            pointLightMH = gfx::MaterialCache::create_material("point light", lightshader);
            pointLightMH.set_param("color", math::colors::red);
            pointLightMH.set_param("intensity", additionalLightIntensity);

            ///////////////////////
            // Surface materials //
            ///////////////////////
            auto pbrShader = gfx::ShaderCache::create_shader("pbr", "assets://shaders/pbr.shs"_view);

            defaultMH = gfx::MaterialCache::create_material("default", pbrShader);
            defaultMH.set_param(SV_ALBEDO, gfx::TextureCache::create_texture("engine://resources/default/albedo"_view));
            defaultMH.set_param(SV_NORMALHEIGHT, gfx::TextureCache::create_texture("engine://resources/default/normalHeight"_view));
            defaultMH.set_param(SV_MRDAO, gfx::TextureCache::create_texture("engine://resources/default/MRDAo"_view));
            defaultMH.set_param(SV_EMISSIVE, gfx::TextureCache::create_texture("engine://resources/default/emissive"_view));
            defaultMH.set_param(SV_HEIGHTSCALE, 1.f);
            defaultMH.set_param("discardExcess", false);

            copperMH = gfx::MaterialCache::create_material("copper", pbrShader);
            copperMH.set_param(SV_ALBEDO, gfx::TextureCache::create_texture("assets://textures/copper/copper-albedo-2048.png"_view));
            copperMH.set_param(SV_NORMALHEIGHT, gfx::TextureCache::create_texture("assets://textures/copper/copper-normalHeight-2048.png"_view));
            copperMH.set_param(SV_MRDAO, gfx::TextureCache::create_texture("assets://textures/copper/copper-MRDAo-2048.png"_view));
            copperMH.set_param(SV_EMISSIVE, gfx::TextureCache::create_texture("assets://textures/copper/copper-emissive-2048.png"_view));
            copperMH.set_param(SV_HEIGHTSCALE, 1.f);
            copperMH.set_param("discardExcess", false);

            graniteMH = gfx::MaterialCache::create_material("granite", pbrShader);
            graniteMH.set_param(SV_ALBEDO, gfx::TextureCache::create_texture("assets://textures/rock/rock-albedo-2048.png"_view));
            graniteMH.set_param(SV_NORMALHEIGHT, gfx::TextureCache::create_texture("assets://textures/rock/rock-normalHeight-2048.png"_view));
            graniteMH.set_param(SV_MRDAO, gfx::TextureCache::create_texture("assets://textures/rock/rock-MRDAo-2048.png"_view));
            graniteMH.set_param(SV_EMISSIVE, gfx::TextureCache::create_texture("assets://textures/rock/rock-emissive-2048.png"_view));
            graniteMH.set_param(SV_HEIGHTSCALE, 1.f);
            graniteMH.set_param("discardExcess", true);

            granite2MH = gfx::MaterialCache::create_material("granite2", pbrShader);
            granite2MH.set_param(SV_ALBEDO, gfx::TextureCache::create_texture("assets://textures/rock/rock-albedo-2048.png"_view));
            granite2MH.set_param(SV_NORMALHEIGHT, gfx::TextureCache::create_texture("assets://textures/rock/rock-normalHeight-2048.png"_view));
            granite2MH.set_param(SV_MRDAO, gfx::TextureCache::create_texture("assets://textures/rock/rock-MRDAo-2048.png"_view));
            granite2MH.set_param(SV_EMISSIVE, gfx::TextureCache::create_texture("assets://textures/rock/rock-emissive-2048.png"_view));
            granite2MH.set_param(SV_HEIGHTSCALE, 1.f);
            granite2MH.set_param("discardExcess", false);

            rockMH = gfx::MaterialCache::create_material("rock", pbrShader);
            rockMH.set_param(SV_ALBEDO, gfx::TextureCache::create_texture("assets://textures/detailedRock/Rock020_1K_Albedo.png"_view));
            rockMH.set_param(SV_NORMALHEIGHT, gfx::TextureCache::create_texture("assets://textures/detailedRock/Rock020_1K_NormalHeight.png"_view));
            rockMH.set_param(SV_MRDAO, gfx::TextureCache::create_texture("assets://textures/detailedRock/Rock020_1K_MRDAo.png"_view));
            rockMH.set_param(SV_EMISSIVE, gfx::TextureCache::create_texture("assets://textures/detailedRock/Rock020_1K_emissive.png"_view));
            rockMH.set_param(SV_HEIGHTSCALE, 1.f);
            rockMH.set_param("discardExcess", true);

            rock2MH = gfx::MaterialCache::create_material("rock2", pbrShader);
            rock2MH.set_param(SV_ALBEDO, gfx::TextureCache::create_texture("assets://textures/detailedRock/Rock020_1K_Albedo.png"_view));
            rock2MH.set_param(SV_NORMALHEIGHT, gfx::TextureCache::create_texture("assets://textures/detailedRock/Rock020_1K_NormalHeight.png"_view));
            rock2MH.set_param(SV_MRDAO, gfx::TextureCache::create_texture("assets://textures/detailedRock/Rock020_1K_MRDAo.png"_view));
            rock2MH.set_param(SV_EMISSIVE, gfx::TextureCache::create_texture("assets://textures/detailedRock/Rock020_1K_emissive.png"_view));
            rock2MH.set_param(SV_HEIGHTSCALE, 1.f);
            rock2MH.set_param("discardExcess", false);
#pragma endregion
        }

        auto [lock, materials] = gfx::MaterialCache::get_all_materials();
        {
            async::readonly_guard guard(lock);

            for (auto& [_, material] : materials)
                if (material.has_param<math::color>("skycolor"))
                    material.set_param("skycolor", math::color(0.1f, 0.3f, 1.0f));
        }
#pragma endregion

#pragma region Creating entities
        {
            auto ent = createEntity();
            // This uses delayed component addition.
            // Meaning that the mesh_renderer will add a mesh_filter component when it gets added to the entity.
            // With a mesh_renderer and a mesh_filter the entity satisfies the mesh_renderable archetype which is used by the renderer to render objects.
            ent.add_component(gfx::mesh_renderer(defaultMH, planeH));

            // This uses the transform archetype, it's basically a common bundling of components.
            ent.add_components<transform>(position(0, 0.01f, 0), rotation(), scale(10));
        }

        {
            auto ent = createEntity();
            ent.add_component(gfx::mesh_renderer(copperMH, planeH));
            ent.add_components<transform>(position(10, 0.01f, 0), rotation(), scale(10));
        }

        {
            auto ent = createEntity();
            ent.add_component(gfx::mesh_renderer(rockMH, planeH));
            ent.add_components<transform>(position(10, 0.01f, 10), rotation(), scale(10));
        }

        {
            auto ent = createEntity();
            ent.add_component(gfx::mesh_renderer(graniteMH, planeH));
            ent.add_components<transform>(position(10, 0.01f, -10), rotation(), scale(10));
        }

        {
            auto ent = createEntity();
            ent.add_component(gfx::mesh_renderer(uvMH, planeH));
            ent.add_components<transform>(position(-10, 0.01f, 0), rotation(), scale(10));
        }

        {
            auto ent = createEntity();
            ent.add_component(gfx::mesh_renderer(normalMH, planeH));
            ent.add_components<transform>(position(-10, 0.01f, 10), rotation(), scale(10));
        }

        {
            auto ent = createEntity();
            ent.add_component(gfx::mesh_renderer(wireframeMH, planeH));
            ent.add_components<transform>(position(0, 0.01f, 10), rotation(), scale(10));
        }

        {
            auto ent = createEntity();
            ent.add_component(gfx::mesh_renderer(vertexColorMH, planeH));
            ent.add_components<transform>(position(0, 0.01f, -10), rotation(), scale(10));
        }

        {
            sun = createEntity();
            sun.add_components(gfx::mesh_renderer(directionalLightMH, directionalLightH));
            sun.add_component<gfx::light>(gfx::light::directional(math::color(1, 1, 0.8f), 10.f));
            sun.add_components<transform>(position(10, 10, 10), rotation::lookat(math::vec3(1, 1, 1), math::vec3::zero), scale());
        }

        {
            auto ent = createEntity();
            ent.add_component(gfx::mesh_renderer(spotLightMH, spotLightH));
            ent.add_component<gfx::light>(gfx::light::spot(math::colors::green, math::deg2rad(45.f), additionalLightIntensity, 50.f));
            ent.add_components<transform>(position(-10, 0.5, -10), rotation::lookat(math::vec3(0, 0, -1), math::vec3::zero), scale());
        }

        {
            auto ent = createEntity();
            ent.add_component(gfx::mesh_renderer(spotLightMH, spotLightH));
            ent.add_component<gfx::light>(gfx::light::spot(math::colors::green, math::deg2rad(45.f), additionalLightIntensity, 50.f));
            ent.add_components<transform>(position(0, 0.5, -10), rotation::lookat(math::vec3(0, 0, -1), math::vec3::zero), scale());
        }

        {
            auto ent = createEntity();
            ent.add_component(gfx::mesh_renderer(spotLightMH, spotLightH));
            ent.add_component<gfx::light>(gfx::light::spot(math::colors::green, math::deg2rad(45.f), additionalLightIntensity, 50.f));
            ent.add_components<transform>(position(10, 0.5, -10), rotation::lookat(math::vec3(0, 0, -1), math::vec3::zero), scale());
        }

        {
            auto ent = createEntity();
            ent.add_component(gfx::mesh_renderer(pointLightMH, pointLightH));
            ent.add_component<gfx::light>(gfx::light::point(math::colors::red, additionalLightIntensity, 50.f));
            ent.add_components<transform>(position(0, 1, 0), rotation(), scale());
        }


        {
            auto ent = createEntity();
            ent.add_component(gfx::mesh_renderer(pointLightMH, pointLightH));
            ent.add_component<gfx::light>(gfx::light::point(math::colors::red, additionalLightIntensity, 50.f));
            ent.add_components<transform>(position(-10, 1, 0), rotation(), scale());
        }


        {
            auto ent = createEntity();
            ent.add_component(gfx::mesh_renderer(pointLightMH, pointLightH));
            ent.add_component<gfx::light>(gfx::light::point(math::colors::red, additionalLightIntensity, 50.f));
            ent.add_components<transform>(position(10, 1, 0), rotation(), scale());
        }


        {
            auto ent = createEntity();
            if (suzanneMaterials.size() > 0)
            {
                ent.add_component(gfx::mesh_renderer(suzanneMaterials[0], suzanneH));
                ent.add_components<use_embedded_material>(use_embedded_material{ suzanneMaterials[0].get_name(), "assets://models/suzanne-test.obj" });
            }
            else
                ent.add_component(gfx::mesh_renderer(gfx::invalid_material_handle, suzanneH));
            ent.add_component<custom_component>();
            ent.add_components<transform>(position(0, 3, 5.1f), rotation(), scale());
        }

        {
            auto ent = createEntity();
            ent.add_component(gfx::mesh_renderer(wireframeMH, suzanneH));
            ent.add_component<custom_component>();
            ent.add_components<transform>(position(0, 3, 8.1f), rotation(), scale());
        }

        {
            auto ent = m_ecs->createEntity();
            ent.add_component(gfx::mesh_renderer(copperMH, suzanneH));
            ent.add_component<custom_component>();

            ent.add_components<transform>(position(0, 3, 11.1f), rotation(), scale());
        }

        {
            auto ent = createEntity();
            ent.add_component(gfx::mesh_renderer(defaultMH, cubeH));
            ent.add_component<custom_component>();
            ent.add_components<transform>(position(5.1f, 9, 0), rotation(), scale());
        }

        {
            auto ent = createEntity();
            ent.add_component(gfx::mesh_renderer(copperMH, sphereH));
            ent.add_component<custom_component>();
            ent.add_components<transform>(position(0, 3, -5.1f), rotation(), scale());
        }

        {
            auto ent = createEntity();
            ent.add_component(gfx::mesh_renderer(granite2MH, sphereH));
            ent.add_component<custom_component>();
            ent.add_components<transform>(position(0, 3, -8.f), rotation(), scale());
        }

        {
            auto ent = createEntity();
            ent.add_component(gfx::mesh_renderer(rock2MH, sphereH));
            ent.add_component<custom_component>();
            ent.add_components<transform>(position(0, 3, -2.2f), rotation(), scale());
        }

        {
            auto ent = createEntity();
            ent.add_component(gfx::mesh_renderer(defaultMH, sphereH));
            ent.add_component<custom_component>();
            ent.add_components<transform>(position(4, 3, -8.f), rotation(), scale());
        }

        {
            auto ent = createEntity();
            ent.add_component(gfx::mesh_renderer(copperMH, uvsphereH));
            ent.add_component<custom_component>();
            ent.add_components<transform>(position(0, 3, -3.6f), rotation(), scale());
        }

        {
            auto ent = createEntity();
            ent.add_component(gfx::mesh_renderer(granite2MH, uvsphereH));
            ent.add_component<custom_component>();
            ent.add_components<transform>(position(0, 3, -6.5f), rotation(), scale());
        }

        {
            auto ent = createEntity();
            ent.add_component(gfx::mesh_renderer(rock2MH, uvsphereH));
            ent.add_component<custom_component>();
            ent.add_components<transform>(position(0, 3, -0.7f), rotation(), scale());
        }

        {
            auto ent = createEntity();
            ent.add_component(gfx::mesh_renderer(defaultMH, uvsphereH));
            ent.add_component<custom_component>();
            ent.add_components<transform>(position(4, 3, -6.5f), rotation(), scale());
        }

#pragma endregion
    }

#pragma region input stuff
    void onLightSwitch(light_switch* action)
    {
        if (GuiTestSystem::isEditingText)
            return;

        static bool on = true;

        if (!action->value)
        {
            if (on)
            {
                if (sun)
                    sun.destroy();

                auto [lock, materials] = gfx::MaterialCache::get_all_materials();
                {
                    async::readonly_guard guard(lock);

                    for (auto& [_, material] : materials)
                        if (material.has_param<math::color>("skycolor"))
                            material.set_param("skycolor", math::color(0.005f, 0.0055f, 0.0065f));
                }
            }
            else
            {
                if (!sun)
                {
                    sun = createEntity(false);
                    sun.add_components<gfx::mesh_renderable>(
                        mesh_filter(MeshCache::get_handle("directional light")),
                        gfx::mesh_renderer(gfx::MaterialCache::get_material("directional light")));

                    sun.add_component<gfx::light>(gfx::light::directional(math::color(1, 1, 0.8f), 10.f));
                    sun.add_components<transform>(position(10, 10, 10), rotation::lookat(math::vec3(1, 1, 1), math::vec3::zero), scale());
                    hierarchy hry;
                    if (SceneManager::currentScene)
                    {
                        hry.parent = SceneManager::currentScene.entity;
                    }
                    else
                        hry.parent = world;

                    auto parentHry = hry.parent.read_component<hierarchy>();
                    parentHry.children.insert(sun);
                    hry.parent.write_component(parentHry);

                    hry.name = "Sun";
                    sun.add_component(hry);
                }

                auto [lock, materials] = gfx::MaterialCache::get_all_materials();
                {
                    async::readonly_guard guard(lock);

                    for (auto& [_, material] : materials)
                        if (material.has_param<math::color>("skycolor"))
                            material.set_param("skycolor", math::color(0.1f, 0.3f, 1.0f));
                }
            }
            on = !on;
        }
    }

    void onTonemapSwitch(tonemap_switch* action)
    {
        if (GuiTestSystem::isEditingText)
            return;

        static gfx::tonemapping_type algorithm = gfx::tonemapping_type::aces;

        if (!action->value)
        {
            switch (algorithm)
            {
            case gfx::tonemapping_type::aces:
                gfx::Tonemapping::setAlgorithm(gfx::tonemapping_type::reinhard);
                algorithm = gfx::tonemapping_type::reinhard;
                log::debug("Reinhard tonemapping");
                break;
            case gfx::tonemapping_type::reinhard:
                gfx::Tonemapping::setAlgorithm(gfx::tonemapping_type::reinhard_jodie);
                algorithm = gfx::tonemapping_type::reinhard_jodie;
                log::debug("Reinhard Jodie tonemapping");
                break;
            case gfx::tonemapping_type::reinhard_jodie:
                gfx::Tonemapping::setAlgorithm(gfx::tonemapping_type::legion);
                algorithm = gfx::tonemapping_type::legion;
                log::debug("Legion tonemapping");
                break;
            case gfx::tonemapping_type::legion:
                gfx::Tonemapping::setAlgorithm(gfx::tonemapping_type::unreal3);
                algorithm = gfx::tonemapping_type::unreal3;
                log::debug("Unreal3 tonemapping");
                break;
            case gfx::tonemapping_type::unreal3:
                gfx::Tonemapping::setAlgorithm(gfx::tonemapping_type::aces);
                algorithm = gfx::tonemapping_type::aces;
                log::debug("ACES tonemapping");
                break;
            default:
                gfx::Tonemapping::setAlgorithm(gfx::tonemapping_type::legion);
                algorithm = gfx::tonemapping_type::legion;
                log::debug("Legion tonemapping");
                break;
            }
        }
    }
#pragma endregion

    void update(time::span deltaTime)
    {
        // Create a component query so we can find all entities with the components we care about.
        // Meaning all the entities we will get back from the query are only the entities that have both the custom_component and rotation in this case.
        // They could have more components than only the ones we care about, but the must at least have the ones we specified.
        // 
        // It's marked static so that it only gets created once and reused every time this function is called.
        static auto componentQuery = createQuery<custom_component, rotation>();

        // Update the query to find all the entities with the components we want.
        componentQuery.queryEntities();

        // Get a list with all the rotation components so that we can change them.
        auto& rotations = componentQuery.get<rotation>();
        float dt = deltaTime; // Minuscule optimization, by doing this the timespan only gets converted to a float once per frame.

        // We can schedule a multi-threaded task that will rotate all our entities.
        // The task we send to the scheduler will be executed as many times as the first parameter.
        // So in this case componentQuery.size() times. The job system will call the function on every available thread until the amount of calls is satisfied.
        // You can get the index of the current call using async::this_job::get_id().
        auto task = m_scheduler->queueJobs(componentQuery.size(), [&]() // <--- If this syntax seems confusing then google: c++ lambda
            {
                id_type idx = async::this_job::get_id(); // Get the current invoke index.
                auto& rot = rotations[idx]; // Get the rotation we are supposed to handle in this job.
                rot = math::angleAxis(math::deg2rad(45.f * dt), rot.up()) * rot; // Rotate the rotation by 45 degrees per second.
            });

        task.wait(); // Wait for all the threads to finish executing my jobs.
        // This also tells the scheduler that our current thread is fine to use to execute jobs on, so it also speeds up finishing our task.

        // Now we need to tell the ECS that we made changes to the rotations, so we submit them.
        componentQuery.submit<rotation>();
    }
};
