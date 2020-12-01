#pragma once
#include <core/core.hpp>
#include <application/application.hpp>
#include <core/filesystem/filesystem.hpp>
#include <rendering/data/material.hpp>
#include <core/logging/logging.hpp>
#include <rendering/debugrendering.hpp>

#include <rendering/components/renderable.hpp>
#include <physics/components/physics_component.hpp>


#include "pointcloud_particlesystem.hpp"
#include <rendering/components/particle_emitter.hpp>

using namespace legion;

class TestSystemConvexHull final : public System<TestSystemConvexHull>
{
public:
    std::shared_ptr<legion::physics::ConvexCollider> collider;

    TestSystemConvexHull()
    {
        log::filter(log::severity::debug);
        app::WindowSystem::requestWindow(world_entity_id, math::ivec2(1360, 768), "LEGION Engine", "Legion Icon", nullptr, nullptr, 1);
    }

    virtual void setup()
    {
        createProcess<&TestSystemConvexHull::update>("Update");

        rendering::model_handle cube;

        rendering::material_handle flatGreen;
        rendering::material_handle vertexColor;
        rendering::material_handle directionalLightMH;
        rendering::material_handle wireFrameH;

        app::window window = m_ecs->world.get_component_handle<app::window>().read();
        {
            async::readwrite_guard guard(*window.lock);
            app::ContextHelper::makeContextCurrent(window);


            auto colorshader = rendering::ShaderCache::create_shader("color", "assets://shaders/color.shs"_view);
            directionalLightMH = rendering::MaterialCache::create_material("directional light", colorshader);
            directionalLightMH.set_param("color", math::color(1, 1, 0.8f));

            cube = rendering::ModelCache::create_model("cube", "assets://models/cube.obj"_view);
            vertexColor = rendering::MaterialCache::create_material("vertex color", "assets://shaders/vertexcolor.shs"_view);
            wireFrameH = rendering::MaterialCache::create_material("wireframe", "assets://shaders/wireframe.shs"_view);

            // Create physics entity
            {
                auto ent = createEntity();
                //ent.add_components<rendering::renderable>({ cube, vertexColor });
                ent.add_components<transform>(position(0, 0, 0), rotation(), scale(1));
                auto pcH = ent.add_component<physics::physicsComponent>();
                auto pc = pcH.read();

                auto mesh = cube.get_mesh();
                collider = pc.ConstructConvexHull(mesh);
            }

        }
    }

    void update(time::span deltaTime)
    {
        auto debugDrawEdges = [](legion::physics::HalfEdgeEdge* edge)
        {
            math::vec3 pos0 = *edge->edgePositionPtr;
            math::vec3 pos1 = *edge->nextEdge->edgePositionPtr;
            debug::drawLine(pos0, pos1, math::colors::red);
        };

        auto faces = collider->GetHalfEdgeFaces();
        for (int i = 0; i < faces.size(); ++i)
        {
            faces.at(i)->forEachEdge(debugDrawEdges);
        }
    }
};
