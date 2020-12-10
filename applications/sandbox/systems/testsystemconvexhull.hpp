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
        rendering::model_handle model;

        rendering::material_handle flatGreen;
        rendering::material_handle vertexColor;
        rendering::material_handle directionalLightMH;
        rendering::material_handle wireFrameH;

        app::window window = m_ecs->world.get_component_handle<app::window>().read();
        {
            async::readwrite_guard guard(*window.lock);
            app::ContextHelper::makeContextCurrent(window);

            cube = rendering::ModelCache::create_model("cube", "assets://models/cube.obj"_view);
            model = rendering::ModelCache::create_model("model", "assets://models/convexhulltest_0.obj"_view);
            wireFrameH = rendering::MaterialCache::create_material("wireframe", "assets://shaders/wireframe.shs"_view);
            vertexColor = rendering::MaterialCache::create_material("vertexColor", "assets://shaders/vertexcolor.shs"_view);

            // Create physics entity
            {
                auto ent = createEntity();
                ent.add_components<rendering::renderable>({ model, wireFrameH });
                ent.add_components<transform>(position(0, 0, 0), rotation(), scale(1));
                auto pcH = ent.add_component<physics::physicsComponent>();
                auto pc = pcH.read();

                auto mesh = model.get_mesh();
                collider = pc.ConstructConvexHull(mesh);
                pcH.write(pc);
               /* auto rbH = ent.add_component<physics::rigidbody>();
                auto rb = rbH.read();
                rb.setMass(1.0f);
                rbH.write(rb);*/
            }
            // Create physics entity
            {
                auto ent = createEntity();
                ent.add_components<rendering::renderable>({ cube, wireFrameH });
                ent.add_components<transform>(position(0, -5, 0), rotation(), scale(2));
                auto pcH = ent.add_component<physics::physicsComponent>();
                auto pc = pcH.read();

                pc.AddBox(physics::cube_collider_params(1.0f, 1.0f, 1.0f));
                pcH.write(pc);
            }
            //// Create entity for reference
            //{
            //    auto ent = createEntity();
            //    ent.add_components<rendering::renderable>({ cube, vertexColor });
            //    ent.add_components<transform>(position(3.5f, 0, 0), rotation(), scale(1));
            //}
            //// Create entity for reference
            //{
            //    auto ent = createEntity();
            //    ent.add_components<rendering::renderable>({ cube, wireFrameH });
            //    ent.add_components<transform>(position(0, 0, -3.5f), rotation(), scale(1));
            //}
        }
    }

    void update(time::span deltaTime)
    {
        auto debugDrawEdges = [](legion::physics::HalfEdgeEdge* edge)
        {
            if (!edge || !edge->nextEdge) return;
            math::vec3 pos0 = edge->edgePosition;
            math::vec3 pos1 = edge->nextEdge->edgePosition;
            debug::drawLine(pos0, pos1, math::colors::red);
        };

        auto faces = collider->GetHalfEdgeFaces();
        for (int i = 0; i < faces.size(); ++i)
        {
            faces.at(i)->forEachEdge(debugDrawEdges);
            //debug::drawLine(faces.at(i)->centroid, faces.at(i)->centroid + faces.at(i)->normal * 0.3f, math::colors::black);
        }
        physics::PhysicsSystem::IsPaused = false;

        //drawPhysicsColliders();
    }

    void drawPhysicsColliders()
    {
        static auto physicsQuery = createQuery< physics::physicsComponent>();

        for (auto entity : physicsQuery)
        {
            auto rotationHandle = entity.get_component_handle<rotation>();
            auto positionHandle = entity.get_component_handle<position>();
            auto scaleHandle = entity.get_component_handle<scale>();
            auto physicsComponentHandle = entity.get_component_handle<physics::physicsComponent>();

            bool hasTransform = rotationHandle && positionHandle && scaleHandle;
            bool hasNecessaryComponentsForPhysicsManifold = hasTransform && physicsComponentHandle;

            if (hasNecessaryComponentsForPhysicsManifold)
            {
                auto rbColor = math::color(0.0, 0.5, 0, 1);
                auto statibBlockColor = math::color(0, 1, 0, 1);

                rotation rot = rotationHandle.read();
                position pos = positionHandle.read();
                scale scale = scaleHandle.read();

                auto usedColor = statibBlockColor;
                bool useDepth = false;

                if (entity.get_component_handle<physics::rigidbody>())
                {
                    usedColor = rbColor;
                    useDepth = true;
                }


                //assemble the local transform matrix of the entity
                math::mat4 localTransform;
                math::compose(localTransform, scale, rot, pos);

                auto physicsComponent = physicsComponentHandle.read();

                for (auto physCollider : *physicsComponent.colliders)
                {
                    //--------------------------------- Draw Collider Outlines ---------------------------------------------//

                    for (auto face : physCollider->GetHalfEdgeFaces())
                    {
                        //face->forEachEdge(drawFunc);
                        physics::HalfEdgeEdge* initialEdge = face->startEdge;
                        physics::HalfEdgeEdge* currentEdge = face->startEdge;

                        math::vec3 faceStart = localTransform * math::vec4(face->centroid, 1);
                        math::vec3 faceEnd = faceStart + math::vec3((localTransform * math::vec4(face->normal, 0)));

                        debug::drawLine(faceStart, faceEnd, math::colors::green, 5.0f);

                        if (!currentEdge) { return; }

                        do
                        {
                            physics::HalfEdgeEdge* edgeToExecuteOn = currentEdge;
                            currentEdge = currentEdge->nextEdge;

                            math::vec3 worldStart = localTransform * math::vec4(edgeToExecuteOn->edgePosition, 1);
                            math::vec3 worldEnd = localTransform * math::vec4(edgeToExecuteOn->nextEdge->edgePosition, 1);

                            debug::drawLine(worldStart, worldEnd, usedColor, 2.0f, 0.0f, useDepth);

                        } while (initialEdge != currentEdge && currentEdge != nullptr);
                    }
                }

            }

        }
    }
};
