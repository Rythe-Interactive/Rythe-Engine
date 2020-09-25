#pragma once
#include <core/core.hpp>
#include <physics/HalfEdgeEdge.h>
#include <application/application.hpp>
#include <core/math/math.hpp>

#include <core/logging/logging.hpp>
#include <physics/physics_component.hpp>
#include <physics/rigidbody.hpp>
#include <physics/cube_collider_params.hpp>
#include <physics/data/physics_manifold_precursor.h>
#include <physics/physicssystem.hpp>
#include <physics/HalfEdgeFace.h>



#include <core/compute/context.hpp>
#include <core/compute/kernel.hpp>

using namespace args;

struct sah
{
    int value;

    sah operator+(const sah& other)
    {
        return { value + other.value };
    }

    sah operator*(const sah& other)
    {
        return { value * other.value };
    }
};

struct physicsIdentifier
{

};

struct player_move : public app::input_axis<player_move> {};
struct player_strive : public app::input_axis<player_strive> {};
struct player_fly : public app::input_axis<player_fly> {};
struct player_look_x : public app::input_axis<player_look_x> {};
struct player_look_y : public app::input_axis<player_look_y> {};

struct exit_action : public app::input_action<exit_action> {};

struct physics_unit_test_move : public app::input_axis<physics_unit_test_move> {};



class TestSystem final : public System<TestSystem>
{
public:
    ecs::entity_handle player;

    std::vector<ecs::entity_handle> physicsUnitTestObjects;

    virtual void setup()
    {
        filter(log::severity::debug);

        compute::Program prog =  fs::view("basic://kernels/vadd_kernel.cl").load_as<compute::Program>();
        prog.prewarm("vector_add");

        std::vector<int> ints;

        auto res = fs::view("basic://bigint.txt").get();
        if (res == common::valid) {

            char* buf = new char[6];
            memset(buf,0,6);
            filesystem::basic_resource contents = res;

            for (size_t i = 0; i < contents.size() && i < 5*2048; i += 5)
            {
                memcpy(buf,contents.data()+ i,5);
                ints.push_back(std::atol(buf));
                
            }

            delete[] buf;
        }

        std::vector<int> first_ints (ints.begin(), ints.begin()+ints.size()/2);
        std::vector<int> second_ints (ints.begin() + ints.size() /2 , ints.end());

        size_t to_process = std::min(first_ints.size(),second_ints.size());

        std::vector<int> results(to_process);


        auto A = compute::Context::createBuffer(first_ints,compute::buffer_type::READ_BUFFER, "A");
        auto B = compute::Context::createBuffer(second_ints,compute::buffer_type::READ_BUFFER, "B");
        auto C = compute::Context::createBuffer(results,compute::buffer_type::WRITE_BUFFER, "C");

         prog.kernelContext("vector_add")
            .set_and_enqueue_buffer(A)
            .set_and_enqueue_buffer(B)
            .set_buffer(C)
            .global(1024)
            .local(64)
            .dispatch()
            .enqueue_buffer(C)
            .finish();



        for (int& i : results)
        {
            log::info("got {}", i);
        }

        app::InputSystem::createBinding<player_move>(app::inputmap::method::W, 1.f);
        app::InputSystem::createBinding<player_move>(app::inputmap::method::S, -1.f);
        app::InputSystem::createBinding<player_strive>(app::inputmap::method::D, 1.f);
        app::InputSystem::createBinding<player_strive>(app::inputmap::method::A, -1.f);
        app::InputSystem::createBinding<player_fly>(app::inputmap::method::SPACE, 1.f);
        app::InputSystem::createBinding<player_fly>(app::inputmap::method::LEFT_SHIFT, -1.f);
        app::InputSystem::createBinding<player_look_x>(app::inputmap::method::MOUSE_X, 0.f);
        app::InputSystem::createBinding<player_look_y>(app::inputmap::method::MOUSE_Y, 0.f);
        app::InputSystem::createBinding<exit_action>(app::inputmap::method::ESCAPE);

        app::InputSystem::createBinding< physics_unit_test_move>(app::inputmap::method::RIGHT, 1.0f);
        app::InputSystem::createBinding< physics_unit_test_move>(app::inputmap::method::LEFT, -1.0f);


        bindToEvent<player_move, &TestSystem::onPlayerMove>();
        bindToEvent<player_strive, &TestSystem::onPlayerStrive>();
        bindToEvent<player_fly, &TestSystem::onPlayerFly>();
        bindToEvent<player_look_x, &TestSystem::onPlayerLookX>();
        bindToEvent<player_look_y, &TestSystem::onPlayerLookY>();
        bindToEvent<exit_action, &TestSystem::onExit>();
        bindToEvent<physics_unit_test_move, &TestSystem::onPhysicsUnitTest>();

        app::window window = m_ecs->world.get_component_handle<app::window>().read();
        window.enableCursor(false);
        rendering::model_handle modelH;
        rendering::material_handle wireframeH;
        rendering::material_handle vertexH;
        
        {
            async::readwrite_guard guard(*window.lock);
            app::ContextHelper::makeContextCurrent(window);

            modelH = rendering::ModelCache::create_model("test", "assets://models/Cube.obj"_view);
            wireframeH = rendering::MaterialCache::create_material("wireframe", "assets://shaders/wireframe.glsl"_view);
            vertexH = rendering::MaterialCache::create_material("vertex", "assets://shaders/position.glsl"_view);

            app::ContextHelper::makeContextCurrent(nullptr);
        }

        {
            auto ent = m_ecs->createEntity();
            ent.add_component<sah>();
            m_ecs->createComponent<rendering::renderable>(ent, { modelH, wireframeH });

            auto [positionH, rotationH, scaleH] = m_ecs->createComponent<transform>(ent);
            positionH.write(math::vec3(0, 0, 5.1f));
            scaleH.write(math::vec3(1.f));
        }

        {
            auto ent = m_ecs->createEntity();
            ent.add_component<sah>();
            m_ecs->createComponent<rendering::renderable>(ent, { modelH, vertexH });

            auto [positionH, rotationH, scaleH] = m_ecs->createComponent<transform>(ent);
            positionH.write(math::vec3(5.1f, 0, 0));
            scaleH.write(math::vec3(0.75f));
        }

        {
            auto ent = m_ecs->createEntity();
            ent.add_component<sah>();
            m_ecs->createComponent<rendering::renderable>(ent, { modelH, wireframeH });

            auto [positionH, rotationH, scaleH] = m_ecs->createComponent<transform>(ent);
            positionH.write(math::vec3(0, 0, -5.1f));
            scaleH.write(math::vec3(0.5f));
        }

        {
            auto ent = m_ecs->createEntity();
            ent.add_component<sah>();
            m_ecs->createComponent<rendering::renderable>(ent, { modelH, vertexH });

            auto [positionH, rotationH, scaleH] = m_ecs->createComponent<transform>(ent);
            positionH.write(math::vec3(-5.1f, 0, 0));
            scaleH.write(math::vec3(0.25f));
        }

        setupCameraEntity();

        //------------------------------------- Setup entity with rigidbody -------------------------------------------//

        physics::cube_collider_params cubeParams;
        cubeParams.breadth = 2.0f;
        cubeParams.width = 2.0f;
        cubeParams.height = 2.0f;

        //auto physicsEnt = m_ecs->createEntity();

        ////setup rendering for physics ent
        //m_ecs->createComponent<rendering::renderable>(physicsEnt, { modelH, wireframeH });

        //auto [bodyPosition, bodyRotation, bodyScale] = m_ecs->createComponent<transform>(physicsEnt);

        //position bodyP = bodyPosition.read();
        //bodyP.x = 0.0f;
        //bodyP.y = 4.0f;
        //bodyP.z = 5.1f;
        //bodyPosition.write(bodyP);

        ////setup physics component on physics ent
        //auto rbHandle = m_ecs->createComponent<physics::rigidbody>(physicsEnt);
        //auto physicsComponentHandle = m_ecs->createComponent<physics::physicsComponent>(physicsEnt);

        //physics::physicsComponent physicsComponent;
        //physics::physicsComponent::init(physicsComponent);

       

        //physicsComponent.AddBox(cubeParams);

        //physicsComponentHandle.write(physicsComponent);



        //auto rb = rbHandle.read();

        //rb.globalCentreOfMass = bodyP;
        ////rb.addForce( math::vec3(-9, 0, 0));
        //rb.addForceAt(math::vec3(0, 4.5, 5.1f), math::vec3(-100, 0, 0));
        //rb.globalCentreOfMass = bodyP;

        //rbHandle.write(rb);

    


        //---------------------------- Physics Unit Tests -------------------------------------//


        //----------- AABB Test ------------//
        {
            auto ent = m_ecs->createEntity();

            auto entPhyHande = ent.add_component<physics::physicsComponent>();

            physics::physicsComponent physicsComponent2;
            physics::physicsComponent::init(physicsComponent2);


            physicsComponent2.AddBox(cubeParams);
            physicsComponent2.isTrigger = false;
            entPhyHande.write(physicsComponent2);


            auto renderableHandle = m_ecs->createComponent<rendering::renderable>(ent);
            renderableHandle.write({ modelH, wireframeH });

            auto [positionH, rotationH, scaleH] = m_ecs->createComponent<transform>(ent);
            positionH.write(math::vec3(0, -3.0f, 5.0f));
            scaleH.write(math::vec3(1.0f));
        }

        {
            auto ent = m_ecs->createEntity();
            physicsUnitTestObjects.push_back(ent);
            auto entPhyHande = ent.add_component<physics::physicsComponent>();

            physics::physicsComponent physicsComponent2;
            physics::physicsComponent::init(physicsComponent2);


            physicsComponent2.AddBox(cubeParams);
            physicsComponent2.isTrigger = true;
            entPhyHande.write(physicsComponent2);


            auto renderableHandle = m_ecs->createComponent<rendering::renderable>(ent);
            renderableHandle.write({ modelH, wireframeH });

            auto [positionH, rotationH, scaleH] = m_ecs->createComponent<transform>(ent);
            positionH.write(math::vec3(3.0, -2.0f, 4.0f));
            scaleH.write(math::vec3(1.0f));
        }

        //----------- AABB to OBB Test  ------------//
        {
            auto ent = m_ecs->createEntity();

            auto entPhyHande = ent.add_component<physics::physicsComponent>();

            physics::physicsComponent physicsComponent2;
            physics::physicsComponent::init(physicsComponent2);


            physicsComponent2.AddBox(cubeParams);
            physicsComponent2.isTrigger = false;
            entPhyHande.write(physicsComponent2);


            auto renderableHandle = m_ecs->createComponent<rendering::renderable>(ent);
            renderableHandle.write({ modelH, wireframeH });

            auto [positionH, rotationH, scaleH] = m_ecs->createComponent<transform>(ent);
            positionH.write(math::vec3(0, -3.0f, -2.0f));
            scaleH.write(math::vec3(1.0f));
        }

        {
            auto ent = m_ecs->createEntity();
            physicsUnitTestObjects.push_back(ent);
            auto entPhyHande = ent.add_component<physics::physicsComponent>();

            physics::physicsComponent physicsComponent2;
            physics::physicsComponent::init(physicsComponent2);


            physicsComponent2.AddBox(cubeParams);
            physicsComponent2.isTrigger = true;
            entPhyHande.write(physicsComponent2);


            auto renderableHandle = m_ecs->createComponent<rendering::renderable>(ent);
            renderableHandle.write({ modelH, wireframeH });

            auto [positionH, rotationH, scaleH] = m_ecs->createComponent<transform>(ent);
            positionH.write(math::vec3(3.0, -3.0f, -2.0f));

            auto rot = rotationH.read();
            rot *= math::angleAxis(45.f , math::vec3(0, 1, 0));
            rot *= math::angleAxis(45.f, math::vec3(0, 0, 1));

            rotationH.write(rot);

            scaleH.write(math::vec3(1.0f));
        }

        //----------- OBB to OBB Test  ------------//
        {
            auto ent = m_ecs->createEntity();

            auto entPhyHande = ent.add_component<physics::physicsComponent>();

            physics::physicsComponent physicsComponent2;
            physics::physicsComponent::init(physicsComponent2);


            physicsComponent2.AddBox(cubeParams);
            physicsComponent2.isTrigger = false;
            entPhyHande.write(physicsComponent2);


            auto renderableHandle = m_ecs->createComponent<rendering::renderable>(ent);
            renderableHandle.write({ modelH, wireframeH });

            auto [positionH, rotationH, scaleH] = m_ecs->createComponent<transform>(ent);
            positionH.write(math::vec3(0, -3.0f, -7.0f));

            auto rot = rotationH.read();
            rot *= math::angleAxis(20.f, math::vec3(0, 1, 0));
            rotationH.write(rot);

            scaleH.write(math::vec3(1.0f));
        }

        {
            auto ent = m_ecs->createEntity();
            physicsUnitTestObjects.push_back(ent);
            auto entPhyHande = ent.add_component<physics::physicsComponent>();

            physics::physicsComponent physicsComponent2;
            physics::physicsComponent::init(physicsComponent2);


            physicsComponent2.AddBox(cubeParams);
            physicsComponent2.isTrigger = true;
            entPhyHande.write(physicsComponent2);


            auto renderableHandle = m_ecs->createComponent<rendering::renderable>(ent);
            renderableHandle.write({ modelH, wireframeH });

            auto [positionH, rotationH, scaleH] = m_ecs->createComponent<transform>(ent);
            positionH.write(math::vec3(3.0, -3.0f, -7.0f));

            auto rot = rotationH.read();
            rot *= math::angleAxis(45.f, math::vec3(0, 1, 0));
            rot *= math::angleAxis(45.f, math::vec3(0, 0, 1));

            rotationH.write(rot);

            scaleH.write(math::vec3(1.0f));
        }



        createProcess<&TestSystem::update>("Update");
        createProcess<&TestSystem::differentThread>("TestChain");

        createProcess<&TestSystem::drawInterval>("Physics",0.01);

        createProcess<&TestSystem::differentInterval>("TestChain", 1.f);
        

        
    }

    void setupCameraEntity()
    {
        player = m_ecs->createEntity();
        auto [camPosHandle, camRotHandle, camScaleHandle] = m_ecs->createComponent<transform>(player);

        rotation rot = camRotHandle.read();
        rot = math::conjugate(math::normalize(math::toQuat(math::lookAt(math::vec3(0, 0, 0), math::vec3(0, 0, 1), math::vec3(0, 1, 0)))));
        camRotHandle.write(rot);


        auto camH = m_ecs->createComponent<rendering::camera>(player);
        rendering::camera cam = camH.read();

        cam.set_projection(60.f, 1360.f / 768.f, 0.1f, 1000.f);
        camH.write(cam);
    }

    void onExit(exit_action* action)
    {
        raiseEvent<events::exit>();
    }

    void onPlayerMove(player_move* action)
    {
        auto posH = player.get_component_handle<position>();
        auto rot = player.get_component_handle<rotation>().read();
        math::vec3 move = math::toMat3(rot) * math::vec3(0.f, 0.f, 1.f);
        move = math::normalize(move * math::vec3(1, 0, 1)) * action->value * action->input_delta * 6.f;
        posH.fetch_add(move);
        //log::debug("FORWD: ({:.3}, {:.3}, {:.3})", move.x, move.y, move.z);
    }

    void onPlayerStrive(player_strive* action)
    {
        auto posH = player.get_component_handle<position>();
        auto rot = player.get_component_handle<rotation>().read();
        math::vec3 move = math::toMat3(rot) * math::vec3(1.f, 0.f, 0.f);
        move = math::normalize(move * math::vec3(1, 0, 1)) * action->value * action->input_delta * 6.f;
        posH.fetch_add(move);
        //log::debug("RIGHT: ({:.3}, {:.3}, {:.3})", move.x, move.y, move.z);
    }

    void onPlayerFly(player_fly* action)
    {
        auto posH = player.get_component_handle<position>();
        posH.fetch_add(math::vec3(0.f, action->value * action->input_delta * 4.f, 0.f));
    }

    void onPlayerLookX(player_look_x* action)
    {
        auto rotH = player.get_component_handle<rotation>();
        rotH.fetch_multiply(math::angleAxis(action->value, math::vec3(0, 1, 0)));
        rotH.read_modify_write(rotation(), [](const rotation& src, rotation&& dummy)
            {
                (void)dummy;
                math::vec3 fwd = math::toMat3(src) * math::vec3(0.f, 0.f, 1.f);
                math::vec3 right = math::cross(fwd, math::vec3(0.f, 1.f, 0.f));
                return (rotation)math::conjugate(math::toQuat(math::lookAt(math::vec3(0.f, 0.f, 0.f), fwd, math::cross(right, fwd))));
            });
    }

    void onPlayerLookY(player_look_y* action)
    {
        auto rotH = player.get_component_handle<rotation>();
        rotH.fetch_multiply(math::angleAxis(action->value, math::vec3(1, 0, 0)));
        rotH.read_modify_write(rotation(), [](const rotation& src, rotation&& dummy)
            {
                (void)dummy;
                math::vec3 fwd = math::toMat3(src) * math::vec3(0.f, 0.f, 1.f);
                math::vec3 right = math::cross(fwd, math::vec3(0.f, 1.f, 0.f));
                return (rotation)math::conjugate(math::toQuat(math::lookAt(math::vec3(0.f, 0.f, 0.f), fwd, math::cross(right, fwd))));
            });
    }

    void onPhysicsUnitTest(physics_unit_test_move* action)
    {
        for (auto entityHandle : physicsUnitTestObjects)
        {
            auto posH = entityHandle.get_component_handle<position>();
            math::vec3 move = math::vec3(1, 0, 0) * action->value * action->input_delta * 2.f;
            posH.fetch_add(move);
        }
    }


    void update(time::span deltaTime)
    {
        debug::drawLine(math::vec3(0, -1, 0), math::vec3(0, -1, 5), math::colors::black);
        debug::drawLine(math::vec3(0, 0, 0), math::vec3(0, 1, 0), math::colors::black);
        debug::drawLine(math::vec3(1, 0, 0), math::vec3(0.5, 1, 0), math::colors::red, 1, false);
        debug::drawLine(math::vec3(1, 0, 0), math::vec3(0, 0, 1), math::colors::green, 10, true);
        debug::drawLine(math::vec3(1, 0, 0), math::vec3(0, 3, 1), math::colors::yellow, 4, false);

        //log::info("still alive! {}",deltaTime.seconds());
        static auto query = createQuery<sah>();
       

        
        //static time::span buffer;
        static int frameCount;
        //static time::span accumulated;

        //buffer += deltaTime;
        //accumulated += deltaTime;
        frameCount++;

        for (auto entity : query)
        {
            auto comp = entity.get_component_handle<sah>();

            auto rot = entity.read_component<rotation>();

            rot *= math::angleAxis(math::deg2rad(45.f * deltaTime), math::vec3(0, 1, 0));

            entity.write_component(rot);

            comp.write({ frameCount });
        }

        //if (buffer > 1.f)
        //{
        //    buffer -= 1.f;

        //    for (auto entity : query)
        //    {
        //        auto comp = entity.get_component_handle<sah>();
        //        std::cout << "component value: " << comp.read().value << std::endl;
        //    }

        //    std::cout << "Hi! " << (frameCount / accumulated) << "fps " << deltaTime.milliseconds() << "ms" << std::endl;
        //}
    }

    void drawInterval(time::span deltaTime)
    {


        static auto physicsQuery = createQuery< physics::physicsComponent>();
        int i = 0;
        //this is called so that i can draw stuff
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
                rotation rot = rotationHandle.read();
                position pos = positionHandle.read();
                scale scale = scaleHandle.read();

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

                        //debug::drawLine(faceStart, faceEnd, math::colors::green, 5.0f);

                        if (!currentEdge) { return; }

                        //log::debug(("---- face"));
                        do
                        {
                            //log::debug(("edge"));
                            physics::HalfEdgeEdge* edgeToExecuteOn = currentEdge;
                            currentEdge = currentEdge->nextEdge;

                            math::vec3 worldStart = localTransform * math::vec4(*(edgeToExecuteOn->edgePositionPtr), 1);
                            math::vec3 worldEnd = localTransform * math::vec4(*(edgeToExecuteOn->nextEdge->edgePositionPtr), 1);

                            debug::drawLine(worldStart, worldEnd, math::vec4(0, 1, 0, 1), 5.0f);
                            i++;
                        } while (initialEdge != currentEdge && currentEdge != nullptr);
                    }




                    //----------------- draw collisions --------------//
                    for (auto line : physCollider->collisionsFound)
                    {
                        debug::drawLine(line.start, line.end, math::vec4(1,0,0,1), 20.0f);
                    }

                    physCollider->collisionsFound.clear();

                }

            }

        }

        //log::debug("{:.3f}", deltaTime);
    }

    void differentInterval(time::span deltaTime)
    {
        static time::span buffer;
        static int frameCount;
        static time::span accumulated;

        buffer += deltaTime;
        accumulated += deltaTime;
        frameCount++;

        math::vec2 v;
        v.x = 10;
        v.y = 20;

        if (buffer > 1.f)
        {
            buffer -= 1.f;
            //std::cout << "This is a fixed interval!! " << (frameCount / accumulated) << "fps " << deltaTime.milliseconds() << "ms" << std::endl;
        }
    }

    void differentThread(time::span deltaTime)
    {
        static auto query = createQuery<sah>();

        static time::span buffer;
        static int frameCount;
        static time::span accumulated;

        buffer += deltaTime;
        accumulated += deltaTime;
        frameCount++;

        /* if (buffer > 1.f)
         {
             buffer -= 1.f;

             for (auto entity : query)
             {
                 auto comp = entity.get_component_handle<sah>();
                 std::cout << "component value on different thread: " << comp.read().value << std::endl;
             }

             std::cout << "This is a different thread!! " << (frameCount / accumulated) << "fps " << deltaTime.milliseconds() << "ms" << std::endl;
         }*/

         //if (accumulated > 10.f)
         //{
         //	std::cout << "raising exit event" << std::endl;
         //	raiseEvent<events::exit>();
         //	//throw args_exception_msg("hehehe fuck you >:D");
         //}
    }
};
