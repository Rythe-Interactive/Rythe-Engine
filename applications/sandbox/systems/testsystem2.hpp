#pragma once
#include <core/core.hpp>
#include <application/application.hpp>
#include <core/filesystem/filesystem.hpp>
#include <rendering/data/material.hpp>
#include <core/logging/logging.hpp>
#include <rendering/debugrendering.hpp>

#include <rendering/components/renderable.hpp>


#include "pointcloud_particlesystem.hpp"
#include "explosion_particlesystem.hpp"
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
        rendering::model_handle sphere;
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

            sphere = rendering::ModelCache::create_model("cube", "assets://models/explosionMesh.obj"_view);
            vertexColor = rendering::MaterialCache::create_material("vertex color", "assets://shaders/color.shs"_view);
            auto pos = sphere.get_mesh().get().second.vertices;
            vertexColor.set_param("color", math::color(227, 86, 28));

            pointCloudParameters params{
            math::vec3(0.2f),
            vertexColor,
                sphere
            };
            auto pointcloud = rendering::ParticleSystemCache::createParticleSystem<PointCloudParticleSystem>("point_cloud", params, pos);


            explosionParameters explosionParams{
                math::vec3(0.2f),
                vertexColor,
                sphere,
                300.0f,
                math::vec3(0.2f),
                0.999f,
                0.99f,
                math::colors::yellow
            };

            auto explosion = rendering::ParticleSystemCache::createParticleSystem<ExplosionParticleSystem>("explosion", explosionParams);

#pragma region entities

            {
                auto ent = createEntity();
                ent.add_components<transform>(position(-5, 0.01f, 0), rotation(), scale());
                //rendering::particle_emitter emitter = ent.add_component<rendering::particle_emitter>().read();
                //emitter.particleSystemHandle = pointcloud;
                //ent.get_component_handle<rendering::particle_emitter>().write(emitter);
            }

            {
                auto ent = createEntity();
                ent.add_components<transform>(position(0, 3.0f, 3.0f), rotation(), scale());
                rendering::particle_emitter emitter = ent.add_component<rendering::particle_emitter>().read();
                emitter.particleSystemHandle = explosion;
                ent.get_component_handle<rendering::particle_emitter>().write(emitter);
            }

#pragma endregion

        }
    }

    virtual void update()
    {

    }
};
