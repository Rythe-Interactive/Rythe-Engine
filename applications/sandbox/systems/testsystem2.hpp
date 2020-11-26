#pragma once
#include <core/core.hpp>
#include <application/application.hpp>
#include <core/filesystem/filesystem.hpp>
#include <rendering/data/material.hpp>
#include <core/logging/logging.hpp>
#include <rendering/debugrendering.hpp>

#include <rendering/components/renderable.hpp>


#include "pointcloud_particlesystem.hpp"
#include <rendering/components/particle_emitter.hpp>

using namespace legion;

class TestSystem2 final : public System<TestSystem2>
{
public:
    TestSystem2()
    {
        log::filter(log::severity::debug);
        app::WindowSystem::requestWindow(world_entity_id, math::ivec2(1360, 768), "LEGION Engine", "Legion Icon", nullptr, nullptr, 1);
    }

    virtual void setup()
    {
        rendering::model_handle cube;
        rendering::material_handle flatGreen;
        rendering::material_handle vertexColor;
        rendering::material_handle directionalLightMH;


        app::window window = m_ecs->world.get_component_handle<app::window>().read();

        {
            async::readwrite_guard guard(*window.lock);
            app::ContextHelper::makeContextCurrent(window);


            auto colorshader = rendering::ShaderCache::create_shader("color", "assets://shaders/color.shs"_view);
            directionalLightMH = rendering::MaterialCache::create_material("directional light", colorshader);
            directionalLightMH.set_param("color", math::color(1, 1, 0.8f));

            cube = rendering::ModelCache::create_model("cube", "assets://models/cube.obj"_view);
            vertexColor = rendering::MaterialCache::create_material("vertex color", "assets://shaders/vertexcolor.shs"_view);


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
            auto pointcloud = rendering::ParticleSystemCache::createParticleSystem<PointCloudParticleSystem>("point_cloud",params, positions);

#pragma region entities

            {
                auto ent = createEntity();
                ent.add_components<transform>(position(-5, 0.01f, 0), rotation(), scale(1));
                rendering::particle_emitter emitter =  ent.add_component<rendering::particle_emitter>().read();
                emitter.particleSystemHandle = pointcloud;
                ent.get_component_handle<rendering::particle_emitter>().write(emitter);
            }

#pragma endregion

        }
    }

    virtual void update()
    {
        
    }
};
