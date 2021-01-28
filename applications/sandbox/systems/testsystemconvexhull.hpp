#pragma once
#include <core/core.hpp>
#include <application/application.hpp>
#include <core/filesystem/filesystem.hpp>
#include <rendering/data/material.hpp>
#include <core/logging/logging.hpp>
#include <rendering/debugrendering.hpp>

#include <rendering/components/renderable.hpp>
#include <physics/components/physics_component.hpp>
#include <physics/components/fracturer.hpp>

#include <rendering/components/particle_emitter.hpp>
#include <physics/halfedgeedge.hpp>
using namespace legion;

struct physics_step : public app::input_action<physics_step> {};
struct collider_move : public app::input_axis<collider_move> {};
struct collider_move_up : public app::input_axis<collider_move_up> {};
struct uniform_grid_broad_phase : public app::input_action<uniform_grid_broad_phase> {};
struct uniform_grid_broad_phase_large : public app::input_action<uniform_grid_broad_phase_large> {};

struct followerData
{
    math::vec3 offset;
};

class TestSystemConvexHull final : public System<TestSystemConvexHull>
{
public:
    std::shared_ptr<legion::physics::ConvexCollider> collider = nullptr;

    core::ecs::entity_handle physicsEnt;
    core::ecs::entity_handle colliderEnt;
    std::vector< core::ecs::entity_handle> followerObjects;

    core::mesh_handle meshH;
    int pStep = 0;

    virtual void setup()
    {
        physics::constants::gravity = math::vec3::zero;

        app::InputSystem::createBinding<physics_step>(app::inputmap::method::ENTER);
        app::InputSystem::createBinding<collider_move>(app::inputmap::method::LEFT, -1);
        app::InputSystem::createBinding<collider_move>(app::inputmap::method::RIGHT, 1);
        app::InputSystem::createBinding<collider_move_up>(app::inputmap::method::UP, 1);
        app::InputSystem::createBinding<collider_move_up>(app::inputmap::method::DOWN, -1);
        app::InputSystem::createBinding<uniform_grid_broad_phase>(app::inputmap::method::K);
        app::InputSystem::createBinding<uniform_grid_broad_phase_large>(app::inputmap::method::L);

        bindToEvent<physics_step, &TestSystemConvexHull::physicsStep>();
        bindToEvent<collider_move, &TestSystemConvexHull::colliderMove>();
        bindToEvent<collider_move_up, &TestSystemConvexHull::colliderMoveUp>();
        bindToEvent<uniform_grid_broad_phase, &TestSystemConvexHull::setUniformGrid>();
        bindToEvent<uniform_grid_broad_phase_large, &TestSystemConvexHull::setUniformGridLarge>();

        createProcess<&TestSystemConvexHull::update>("Update");

        rendering::model_handle cube;
        rendering::model_handle model;

        rendering::material_handle flatGreen;
        rendering::material_handle vertexColor;
        rendering::material_handle directionalLightMH;
        rendering::material_handle wireFrameH;
        rendering::material_handle solidLegion;

        app::window window = m_ecs->world.get_component_handle<app::window>().read();
        {
            app::context_guard guard(window);

            cube = rendering::ModelCache::create_model("cube", "assets://models/cube.obj"_view);
            model = rendering::ModelCache::create_model("model", "assets://models/sphere.obj"_view);
            wireFrameH = rendering::MaterialCache::create_material("wireframe", "assets://shaders/wireframe.shs"_view);
            vertexColor = rendering::MaterialCache::create_material("vertexColor", "assets://shaders/vertexcolor.shs"_view);

            solidLegion = rendering::MaterialCache::create_material("texture", "assets://shaders/texture.shs"_view);
            solidLegion.set_param("_texture", rendering::TextureCache::create_texture("assets://textures/split-test.png"_view));

            // Create physics entity
            /*{
                physicsEnt = createEntity();
                physicsEnt.add_components<rendering::mesh_renderable>(mesh_filter(model.get_mesh()), rendering::mesh_renderer(wireFrameH));
                physicsEnt.add_components<transform>(position(0.0f, 4, 0), rotation(), scale(1));
                meshH = model.get_mesh();

                physicsEnt.add_component<physics::physicsComponent>();
                auto rbH = physicsEnt.add_component<physics::rigidbody>();
                auto rb = rbH.read();
                rb.setMass(1.0f);
                rbH.write(rb);
            }*/

            {
                colliderEnt = createEntity();
                colliderEnt.add_components<rendering::mesh_renderable>(mesh_filter(cube.get_mesh()), rendering::mesh_renderer(solidLegion));
                colliderEnt.add_components<transform>(position(0,1.0f, 0), rotation(), scale(1));
                auto physH = colliderEnt.add_component<physics::physicsComponent>();
                auto p = physH.read();
                p.AddBox(physics::cube_collider_params(1.0f, 1.0f, 1.0f));
                physH.write(p);
                colliderEnt.add_component<physics::rigidbody>();
            }

            {
                auto ent = createEntity();
                ent.add_components<rendering::mesh_renderable>(mesh_filter(cube.get_mesh()), rendering::mesh_renderer(solidLegion));
                ent.add_components<transform>(position(0, 3.0f, 0), rotation(), scale(1));
                auto physH = ent.add_component<physics::physicsComponent>();
                auto p = physH.read();
                p.AddBox(physics::cube_collider_params(1.0f, 1.0f, 1.0f));
                physH.write(p);
                ent.add_component<physics::rigidbody>();
            }

            {
                auto ent = createEntity();
                ent.add_components<rendering::mesh_renderable>(mesh_filter(cube.get_mesh()), rendering::mesh_renderer(solidLegion));
                ent.add_components<transform>(position(0, 5.0f, 0), rotation(), scale(1));
                physics::physicsComponent p;
                p.AddBox(physics::cube_collider_params(1.0f, 1.0f, 1.0f));
                ent.add_component(p);
                ent.add_component<physics::rigidbody>();
            }

#if 1
            for (int i = 0; i < 1000; ++i)
            {
                auto ent = createEntity();
                ent.add_components<rendering::mesh_renderable>(mesh_filter(cube.get_mesh()), rendering::mesh_renderer(solidLegion));
                ent.add_components<transform>(position(math::linearRand(math::vec3(-10, 0, -10), math::vec3(10, 20, 10))), rotation(math::angleAxis(math::linearRand(-math::pi<float>(), math::pi<float>()), math::normalize(math::linearRand(-math::vec3::one, math::vec3::one)))), scale(1.f));

                physics::physicsComponent p;
                p.AddBox(physics::cube_collider_params(1.0f, 1.0f, 1.0f));
                ent.add_component(p);

                auto rbH = ent.add_component<physics::rigidbody>();
            }
#endif
        }
    }

    bool isUpdating = false;

    void update(time::span deltaTime)
    {
        auto [posH, rotH, scaleH] = physicsEnt.get_component_handles<transform>();

        if (!isUpdating)
        {
            for (auto ent : followerObjects)
            {
                ent.write_component<position>(posH.read());
                ent.write_component<rotation>(rotH.read());
                ent.write_component<scale>(scaleH.read());
            }
        }
    }
    
    int stepToSee = 0;
    math::vec3 spacing = math::vec3(2.5f, 0, 0);
    int indexToSee = 3;

    void physicsStep(physics_step* action)
    {
        if (action->value)
        {
            physics::PhysicsSystem::IsPaused = !physics::PhysicsSystem::IsPaused;
        }
        /*else
        {
            physics::PhysicsSystem::IsPaused = true;
        }*/
    }

    void colliderMove(collider_move* action)
    {
        auto posH = colliderEnt.get_component_handle<position>();
        math::vec3 move = math::vec3(1.f, 0, 0);
        move = move * action->value * action->input_delta * 10.f;
        posH.fetch_add(move);
    }

    void colliderMoveUp(collider_move_up* action)
    {
        auto posH = colliderEnt.get_component_handle<position>();
        math::vec3 move = math::vec3(0, 1.f, 0);
        move = move * action->value * action->input_delta * 10.f;
        posH.fetch_add(move);
    }

    void setUniformGrid(uniform_grid_broad_phase* action)
    {
        if (action->value)
        {
            physics::PhysicsSystem::setBroadPhaseCollisionDetection<physics::BroadphaseUniformGrid>(math::ivec3(2, 2, 2));
            log::debug("Set broad phase 2x2x2");
        }
    }

    void setUniformGridLarge(uniform_grid_broad_phase_large* action)
    {
        if (action->value)
        {
            physics::PhysicsSystem::setBroadPhaseCollisionDetection<physics::BroadphaseUniformGrid>(math::ivec3(3, 3, 3));
            log::debug("Set broad phase 3x3x3");
        }
    }

    void drawPhysicsColliders()
    {
        static auto physicsQuery = createQuery< physics::physicsComponent>();
        physicsQuery.queryEntities();

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

                auto usedColor = rbColor;
                bool useDepth = false;

                if (entity.get_component_handle<physics::rigidbody>())
                {
                    usedColor = rbColor;
                    //useDepth = true;
                }


                //assemble the local transform matrix of the entity
                math::mat4 localTransform;
                math::compose(localTransform, scale, rot, pos);

                auto physicsComponent = physicsComponentHandle.read();

                for (auto physCollider : physicsComponent.colliders)
                {
                    //--------------------------------- Draw Collider Outlines ---------------------------------------------//
                    if (!physCollider->shouldBeDrawn) { continue; }
                    //math::vec3 colliderCentroid = pos + math::vec3(localTransform * math::vec4(physCollider->GetLocalCentroid(), 0));
                    //debug::user_projectDrawLine(colliderCentroid, colliderCentroid + math::vec3(0.0f,0.2f,0.0f), math::colors::cyan, 6.0f,0.0f,true);
                    
                    for (auto face : physCollider->GetHalfEdgeFaces())
                    {
                        //face->forEachEdge(drawFunc);
                        physics::HalfEdgeEdge* initialEdge = face->startEdge;
                        physics::HalfEdgeEdge* currentEdge = face->startEdge;

                        math::vec3 faceStart = localTransform * math::vec4(face->centroid, 1);
                        math::vec3 faceEnd = faceStart + math::vec3((localTransform * math::vec4(face->normal, 0))) * 0.1f;
                        
                        //debug::user_projectDrawLine(faceStart, faceEnd, math::colors::green, 2.0f);

                        if (!currentEdge) { return; }

                        do
                        {
                            physics::HalfEdgeEdge* edgeToExecuteOn = currentEdge;
                            currentEdge = currentEdge->nextEdge;

                            math::vec3 worldStart = localTransform * math::vec4(edgeToExecuteOn->edgePosition, 1);
                            math::vec3 worldEnd = localTransform * math::vec4(edgeToExecuteOn->nextEdge->edgePosition, 1);

                            debug::user_projectDrawLine(worldStart, worldEnd, usedColor, 2.0f, 0.0f, useDepth);

                        } while (initialEdge != currentEdge && currentEdge != nullptr);
                    }
                }

            }

        }
    }

    void meshDrawCollider(physics::physicsComponent& comp)
    {
        for (auto ent : followerObjects)
        {
            m_ecs->destroyEntity(ent);
        }
        followerObjects.clear();

        if (collider == nullptr)
        {
            collider = comp.ConstructConvexHull(meshH);
            PopulateFollowerList();
        }
        else
        {
            collider->shouldBeDrawn = false;
            comp.ConstructConvexHull(meshH, *collider);
            collider->shouldBeDrawn = true;
            PopulateFollowerList();
        }
    }

    void vertexDrawCollider(std::vector<math::vec3> vertices,math::vec3 spacing = math::vec3())
    {
        for (auto ent : followerObjects)
        {
            m_ecs->destroyEntity(ent);
        }
        followerObjects.clear();

        collider->shouldBeDrawn = false;
        collider->ConstructConvexHullWithVertices(vertices, spacing);
        collider->shouldBeDrawn = true;
        PopulateFollowerList();
        collider->step++;

    }

    void PopulateFollowerList()
    {
        app::window window = m_ecs->world.get_component_handle<app::window>().read();
        
        auto [posH, rotH, scaleH] = physicsEnt.get_component_handles<transform>();
        //const math::mat4& trans = math::compose(scaleH.read(), rotH.read(), posH.read());

        for (auto face : collider->GetHalfEdgeFaces())
        {
            
            log::debug("object draw");
            math::vec3 localCentroid = face->centroid;
            std::vector<math::vec3> localVert;

            auto populateVectorLambda = [&localVert](physics::HalfEdgeEdge* edge)
            {
                log::debug("edge {}", math::to_string( edge->edgePosition));
                localVert.push_back(edge->edgePosition);
            };

            face->forEachEdge(populateVectorLambda);

            legion::core::mesh newMesh;
            
            std::vector<math::vec3> vertices;
            std::vector<uint> indices;
            std::vector<math::vec2> uvs;

            /*vertices.push_back(localCentroid);
            vertices.push_back(localCentroid + math::vec3(0,1,0));
            vertices.push_back(localCentroid + math::vec3(0, 0, 1));*/
            for (size_t i = 0; i < localVert.size(); i++)
            {
                vertices.push_back(localVert.at(i));
                vertices.push_back(localVert.at((i+1)%localVert.size()));
                log::debug("using {},{}", i,(i+1));
                vertices.push_back(localCentroid);
            }

            for (size_t i = 0; i < vertices.size(); i++)
            {
                indices.push_back(i);
            }

            for (size_t i = 0; i < vertices.size(); i++)
            {
                uvs.push_back(math::vec2());
            }

            newMesh.vertices = vertices;
            newMesh.indices = indices;
            newMesh.uvs = uvs;

            sub_mesh newSubMesh;
            newSubMesh.indexCount = newMesh.indices.size();
            newSubMesh.indexOffset = 0;

            newMesh.submeshes.push_back(newSubMesh);

            //creaate modelH
            static int count = 0;
            mesh_handle meshH = core::MeshCache::create_mesh("meshh" + std::to_string(count), newMesh);
            auto modelH = rendering::ModelCache::create_model(meshH);
            count++;

            //Create Entity
            auto newEnt = m_ecs->createEntity();
            newEnt.add_components<transform>(posH.read(), rotH.read(), scaleH.read());
            //debug::user_projectdrawLine(posH.read(), posH.read() + math::vec3(0, 1, 0), math::colors::magenta, 20.0f, 20.0f);

            //Create Material
            rendering::material_handle newMat;
            {
                app::context_guard guard(window);
                auto colorShader = rendering::ShaderCache::create_shader("color" + std::to_string(count), "assets://shaders/color.shs"_view);
                newMat = rendering::MaterialCache::create_material("vertex color" + std::to_string(count), colorShader);
                newMat.set_param("color", math::color(math::linearRand(0.25f, 0.7f), math::linearRand(0.25f, 0.7f), math::linearRand(0.25f, 0.7f)));
            }

            mesh_filter meshFilter = mesh_filter(meshH);
            newEnt.add_components<rendering::mesh_renderable>(meshFilter, rendering::mesh_renderer(newMat));


            followerObjects.push_back(newEnt);
        }
    }

};
