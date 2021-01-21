#pragma once
#include <core/core.hpp>
#include <application/application.hpp>
#include <core/filesystem/filesystem.hpp>
#include <rendering/data/material.hpp>
#include <core/logging/logging.hpp>
#include <rendering/debugrendering.hpp>

#include <rendering/components/renderable.hpp>

#include <rendering/systems/pointcloud_particlesystem.hpp>
#include <rendering/components/particle_emitter.hpp>

#include <physics/physics_statics.hpp>
#include <rendering/util/matini.hpp>


using namespace legion;



////scene loading binds
struct savescene1 : public app::input_action<savescene1> {};
struct savescene2 : public app::input_action<savescene2> {};

struct loadscene1 : public app::input_action<loadscene1> {};
struct loadscene2 : public app::input_action<loadscene2> {};

struct createRandEnt : public app::input_action<createRandEnt> {};
double rnd() { return double(rand()) / RAND_MAX; }
class SceneTestSystem1 final : public System<SceneTestSystem1>
{
public:

    rendering::model_handle cube;
    rendering::material_handle flatGreen;
    rendering::material_handle vertexColor;
    rendering::material_handle directionalLightMH;
    rendering::material_handle color;

    virtual void setup()
    {
#pragma region Inputs
        app::InputSystem::createBinding<savescene1>(app::inputmap::method::F1);
        app::InputSystem::createBinding<savescene2>(app::inputmap::method::F2);
        app::InputSystem::createBinding<loadscene1>(app::inputmap::method::F3);
        app::InputSystem::createBinding<loadscene2>(app::inputmap::method::F4);

        app::InputSystem::createBinding<createRandEnt>(app::inputmap::method::F5);

        bindToEvent<savescene1, &SceneTestSystem1::saveScene1>();
        bindToEvent<savescene2, &SceneTestSystem1::saveScene2>();
        bindToEvent<loadscene1, &SceneTestSystem1::loadScene1>();
        bindToEvent<loadscene2, &SceneTestSystem1::loadScene2>();

        bindToEvent<createRandEnt, &SceneTestSystem1::createRandomEntity>();
#pragma endregion
        app::window window = m_ecs->world.get_component_handle<app::window>().read();
        {
            app::context_guard guard(window);
           /* async::readwrite_guard guard(*window.lock);
            app::ContextHelper::makeContextCurrent(window);*/
            //app::context_guard guard2(nullptr);

            auto colorshader = rendering::ShaderCache::create_shader("color", "assets://shaders/pbr.shs"_view);

            directionalLightMH = rendering::MaterialCache::create_material("directional light", colorshader);
            directionalLightMH.set_param("color", math::color(1, 1, 0.8f));

            cube = rendering::ModelCache::create_model("cube", "assets://models/cube.obj"_view);

            color = rendering::MaterialCache::create_material("texture", "assets://shaders/texture.shs"_view);
            color.set_param("_texture", rendering::TextureCache::create_texture("assets://textures/test-albedo.png"_view));

            //rendering::apply_material_conf(color,"PBR", "assets://textures/color.ini"_view);
           


            vertexColor = rendering::MaterialCache::create_material("vertex color", "assets://shaders/vertexcolor.shs"_view);

#pragma region entities

            {
                auto ent = createEntity();
                ent.add_components<rendering::mesh_renderable>(mesh_filter(cube.get_mesh()), rendering::mesh_renderer(vertexColor));
                ent.add_components<transform>(position(-5, 0.01f, 0), rotation(), scale(1));
            }

            {
                auto ent = createEntity();
                ent.add_components<rendering::mesh_renderable>(mesh_filter(cube.get_mesh()), rendering::mesh_renderer(color));
                ent.add_components<transform>(position(-2, 0.01f, 0), rotation(), scale(1));
            }

#pragma endregion

        }

        createProcess<&SceneTestSystem1::update>("Update");
    }

    void update(time::span deltaTime)
    {

    }

    void saveScene1(savescene1* action)
    {
        if (action->pressed())
        {
            log::debug("Saving scene: Main");
            scenemanagement::SceneManager::create_scene("Main");
            log::debug("Finishded saving scene: Main");
        }
    }
    void saveScene2(savescene2* action)
    {
        if (action->pressed())
        {
            log::debug("Saving scene: Main2");
            scenemanagement::SceneManager::create_scene("Main2");
            log::debug("Finishded saving scene: Main2");
        }
    }
    void loadScene1(loadscene1* action)
    {
        if (action->pressed())
        {
            log::debug("Started loading scene: Main");
            scenemanagement::SceneManager::load_scene("Main");
            log::debug("Finished loading a scene");
        }
    }
    void loadScene2(loadscene2* action)
    {
        if (action->pressed())
        {
            log::debug("Started loading scene: Main");
            scenemanagement::SceneManager::load_scene("Main2");
            log::debug("Finished loading a scene");
        }
    }

    void createRandomEntity(createRandEnt* action)
    {
        cube = rendering::ModelCache::create_model("cube", "assets://models/cube.obj"_view);
        color = rendering::MaterialCache::create_material("texture", "assets://shaders/texture.shs"_view);
        //rendering::apply_material_conf(color,"PBR", "assets://textures/color.ini"_view);

        color.set_param("_texture", rendering::TextureCache::create_texture("assets://textures/test-albedo.png"_view));


        auto ent = createEntity();
        ent.add_components<rendering::mesh_renderable>(mesh_filter(cube.get_mesh()), rendering::mesh_renderer(color));
        ent.add_components<transform>(position(rnd() * 5, 0.01f, rnd() * 5), rotation(), scale(1));
    }
};
