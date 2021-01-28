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

//struct uniform_grid_broad_phase : public app::input_action<uniform_grid_broad_phase> {};

class final_presentation_system : public System<final_presentation_system>
{
public:
    //std::shared_ptr<legion::physics::ConvexCollider> collider = nullptr;

    virtual void setup()
    {
        //Set the amount of gravity.
        physics::constants::gravity = math::vec3(0.0f,-9.8f,0.0f);

        //Setup the input bindings.
        app::InputSystem::createBinding<uniform_grid_broad_phase>(app::inputmap::method::K);

        //bindToEvent<uniform_grid_broad_phase, &final_presentation_system::setUniformGrid>();

        //Add the update function to the update loop.
        createProcess<&final_presentation_system::update>("Update");

        rendering::model_handle floor;
        //rendering::model_handle plane;
        //rendering::model_handle terminal;
        rendering::model_handle house;
        rendering::model_handle cube;

        std::vector<gfx::material_handle> materials;
        gfx::material_handle floor_material;
        //gfx::material_handle plane_material;
        //gfx::material_handle terminal_material;
        gfx::material_handle house_material;
        gfx::material_handle cube_material;

        //Light stuff
        ecs::entity_handle directional_light = createEntity();
        directional_light.add_components<transform>(position(), rotation::lookat(math::vec3(1, 1, 1), math::vec3::zero), scale());
        directional_light.add_component(gfx::light::directional(math::color(1.f, 1.f, 0.8f), 10.f));

        app::window window = m_ecs->world.get_component_handle<app::window>().read();
        {
            app::context_guard guard(window);

            floor = rendering::ModelCache::create_model("floor_final", "assets://models/final_presentation/floor5.glb"_view, materials);
            floor_material = materials[0];
            floor_material.set_param("skycolor", math::color(0.1f, 0.3f, 1.f));

            house = rendering::ModelCache::create_model("house_final", "assets://models/final_presentation/house_1.glb"_view, materials);
            house_material = materials[1];
            house_material.set_param("skycolor", math::color(0.1f, 0.3f, 1.f));

            cube = rendering::ModelCache::create_model("cube_final", "assets://models/final_presentation/house_1.glb"_view, materials);
            cube_material = materials[2];
            cube_material.set_param("skycolor", math::color(0.1f, 0.3f, 1.f));



            /*plane = rendering::ModelCache::create_model("plane_final", "assets://models/final_presentation/plane.glb"_view, materials);
            plane_material = materials[1];
            plane_material.set_param("skycolor", math::color(0.1f,0.3f,1.f));*/

            /*terminal = rendering::ModelCache::create_model("terminal_final", "assets://models/final_presentation/terminal.glb"_view, materials);
            terminal_material = materials[2];
            terminal_material.set_param("skycolor", math::color(0.1f, 0.3f, 1.f));*/


            //plane
            //{
            //    auto ent = createEntity();
            //    ent.add_component(gfx::mesh_renderer(plane_material, plane));
            //    ent.add_components<transform>(position(0, 32.0f, 0), rotation(math::angleAxis(math::pi<float>()*0.5f,math::vec3(1.0f,0.0f,0.0f))), scale());
            //    auto physH = ent.add_component<physics::physicsComponent>();
            //    auto p = physH.read();
            //    p.AddBox(physics::cube_collider_params(64.0f, 10.f, 10.f));
            //    //p.ConstructConvexHull(plane.get_mesh());
            //    physH.write(p);
            //    ent.add_component<physics::rigidbody>();
            //    //ent.add_component<physics::Fracturer>();
            //}

            //house
            {
                auto ent = createEntity();
                ent.add_component(gfx::mesh_renderer(house_material, house));
                ent.add_components<transform>(position(0, 0.2f, 0), rotation(), scale());
                auto physH = ent.add_component<physics::physicsComponent>();
                auto p = physH.read();
                p.ConstructConvexHull(house.get_mesh());
                physH.write(p);
                ent.add_component<physics::rigidbody>();
               // ent.add_component<physics::Fracturer>();
            }

            //cube
            {
                auto ent = createEntity();
                ent.add_component(gfx::mesh_renderer(cube_material, cube));
                ent.add_components<transform>(position(0, 100.0f, 0), rotation(), scale());
                auto physH = ent.add_component<physics::physicsComponent>();
                auto p = physH.read();
                p.AddBox(physics::cube_collider_params(1.f,1.f,1.f));
                //p.ConstructConvexHull(house.get_mesh());

                auto splitterH = ent.add_component<physics::MeshSplitter>();
                auto splitter = splitterH.read();
                splitter.InitializePolygons(ent);
                splitterH.write(splitter);

                physH.write(p);
                ent.add_component<physics::rigidbody>();
                ent.add_component<physics::Fracturer>();
            }


            //floor
            {
                auto ent = createEntity();
                ent.add_component(gfx::mesh_renderer(floor_material, floor));
                ent.add_components<transform>(position(0, 0.1f, 0), rotation(), scale());
                physics::physicsComponent p;
                p.AddBox(physics::cube_collider_params(640.0f, 640.0f, -1.f));
                ent.add_component(p);
            }

            //terminal
            /*{
                auto ent = createEntity();
                ent.add_component(gfx::mesh_renderer(terminal_material, terminal));
                ent.add_components<transform>(position(32.0f, 5.0f, 32.0f), rotation(math::angleAxis(math::pi<float>() * 0.5f, math::vec3(1.0f, 0.0f, 0.0f))), scale(1));
                physics::physicsComponent p;
                p.AddBox(physics::cube_collider_params(10.0f, 10.0f, 10.0f));
                ent.add_component(p);
            }*/
        }
        physics::PhysicsSystem::IsPaused = !physics::PhysicsSystem::IsPaused;
    }

    void update(time::span deltaTime)
    {
        drawPhysicsColliders();
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
};
