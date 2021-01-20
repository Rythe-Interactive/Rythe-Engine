#pragma once
#include <core/core.hpp>
#include <application/application.hpp>
#include <rendering/data/material.hpp>
#include <core/logging/logging.hpp>
#include <imgui/ImGuizmo.h>

#include <rendering/systems/pointcloud_particlesystem.hpp>
#include "explosion_particlesystem.hpp"

#include <rendering/components/particle_emitter.hpp>
#include <imgui/ImGuiFileBrowser.h>
#include <imgui/imnodes.h>

#include <rendering/pipeline/gui/stages/imguirenderstage.hpp>


using namespace legion;

struct activateUpdate : public app::input_action<activateUpdate> {};

void EditTransform(const float* cameraView, const float* cameraProjection, float* matrix, bool editTransformDecomposition);

class TestSystem2 final : public System<TestSystem2>
{
public:
    TestSystem2() : System<TestSystem2>()
    {
        filter(log::severity::debug);
        app::WindowSystem::requestWindow(world_entity_id, math::ivec2(1360, 768), "LEGION Engine", "Legion Icon", nullptr, nullptr, 1);
    }
    ecs::EntityQuery cameraQuery = createQuery<camera, position, rotation, scale>();
    ecs::entity_handle m_cubeent;
    std::vector<std::pair<int, int>> links;
    imgui::filebrowser::ImGuiFileBrowser file_dialog;

    std::vector<bool*> explEmitterActivation;
    std::vector<std::vector<math::vec4>> voronoi;

    virtual void setup()
    {
        app::InputSystem::createBinding<activateUpdate>(app::inputmap::method::COMMA, 1.f);
        bindToEvent<activateUpdate, &TestSystem2::onParticleActivate>();

        rendering::model_handle explosionSphere;
        rendering::model_handle particleSphere;
        rendering::model_handle cube;

        material_handle vertexColor;
        material_handle pointCloudColor;
        material_handle directionalLightMH;


        app::window window = m_ecs->world.get_component_handle<app::window>().read();
        //app::context_guard guard(window);
        {
            app::context_guard guard(window);

            auto colorshader = ShaderCache::create_shader("color", "assets://shaders/color.shs"_view);
            directionalLightMH = MaterialCache::create_material("directional light", colorshader);
            directionalLightMH.set_param("color", math::color(1, 1, 0.8f));

            cube = ModelCache::create_model("cube", "assets://models/cube.obj"_view);;

            explosionSphere = ModelCache::create_model("cube", "assets://models/explosionMesh.obj"_view);
            particleSphere = ModelCache::create_model("cube", "assets://models/particleModel.obj"_view);

            pointCloudColor = MaterialCache::create_material("color2", "assets://shaders/color.shs"_view);
            pointCloudColor.set_param("color", math::color(0, 1, 1));
            vertexColor = MaterialCache::create_material("color1", "assets://shaders/color.shs"_view);
            vertexColor.set_param("color", math::color(227.0f / 255.0f, 86.0f / 255.0f, 28.0f / 255.0f));
            std::vector<math::vec3> positions{
                math::vec3(0,1.0f,0),
                math::vec3(0,1.25f,0),
                math::vec3(0,1.5f,0),
                math::vec3(0,1.75f,0),

                math::vec3(1,1.0f,0),
                math::vec3(1,1.25f,0),
                math::vec3(1,1.5f,0),
                math::vec3(1,1.75f,0),

                math::vec3(1,1.0f,1),
                math::vec3(1,1.25f,1),
                math::vec3(1,1.5f,1),
                math::vec3(1,1.75f,1),

                math::vec3(0,1.0f,1),
                math::vec3(0,1.25f,1),
                math::vec3(0,1.5f,1),
                math::vec3(0,1.75f,1)
            };
            pointCloudParameters params{
            params.startingSize = math::vec3(0.2f),
            params.particleMaterial = vertexColor,
                params.particleModel = cube
            };
            auto pointcloud = ParticleSystemCache::createParticleSystem<PointCloudParticleSystem>("point_cloud", params, positions);

#pragma region entities

            {
                auto ent = createEntity();
                ent.add_components<transform>(position(-5, 0.01f, 0), rotation(), scale(1));
                particle_emitter emitter = ent.add_component<particle_emitter>().read();
                emitter.particleSystemHandle = pointcloud;
                ent.get_component_handle<particle_emitter>().write(emitter);
            }

#pragma endregion

        }
        //std::vector<math::vec3> points;
        //double x, y, z;
        //double width = 10, height = 10, depth = 10;
        //for (int i = 0; i < 40; i++)
        //{
           /* x = -width + rnd() * (width - (-width));
            y = -height + rnd() * (height - (-height));
            z = -depth + rnd() * (depth - (-depth));
            points.push_back(math::vec3(x, y, z));*/
            /* async::readwrite_guard guard(*window.lock);
             app::ContextHelper::makeContextCurrent(window);*/


        

        auto pos = explosionSphere.get_mesh().get().second.vertices;

        pointCloudParameters params{
        math::vec3(0.2f),
        pointCloudColor,
            particleSphere
        };
        auto pointcloud = ParticleSystemCache::createParticleSystem<PointCloudParticleSystem>("point_cloud", params, pos);


        explosionParameters explosionParams{
            math::vec3(2.0f),
            vertexColor,
            particleSphere,
            3.0f,
            math::vec3(0.2f),
            0.99f,
            0.99f,
            math::colors::yellow,
            explosionSphere
        };

        auto explosion = ParticleSystemCache::createParticleSystem<ExplosionParticleSystem>("explosion", explosionParams);
#pragma region entities

        {
            auto ent = createEntity();
            ent.add_components<transform>(position(-5, 0.01f, 0), rotation(), scale());
            rendering::particle_emitter emitter = ent.add_component<particle_emitter>().read();
            emitter.particleSystemHandle = pointcloud;
            ent.get_component_handle<particle_emitter>().write(emitter);
        }

        {
            auto ent = createEntity();
            ent.add_components<transform>(position(0, 3.0f, 3.0f), rotation(), scale(15.f));
            particle_emitter emitter = ent.add_component<particle_emitter>().read();
            emitter.particleSystemHandle = explosion;
            ent.get_component_handle<particle_emitter>().write(emitter);
        }

        /*{
            auto ent = createEntity();
            ent.add_components<transform>(position(2, 3.0f, 3.0f), rotation(), scale(3.f));
            rendering::particle_emitter emitter = ent.add_component<rendering::particle_emitter>().read();
            emitter.particleSystemHandle = explosion;
            ent.get_component_handle<rendering::particle_emitter>().write(emitter);
        }*/

#pragma endregion
        createProcess<&TestSystem2::update>("Update");
    }


    void update(time::span dt)
    {
    }
    
    void onParticleActivate(activateUpdate* action)
    {
        auto emitters = createQuery<particle_emitter>();
        emitters.queryEntities();
        for (auto entity_handle : emitters)
        {
            particle_emitter emit = entity_handle.get_component_handle<particle_emitter>().read();
            emit.playAnimation = true;
            entity_handle.get_component_handle<particle_emitter>().write(emit);
        }
    }
};



