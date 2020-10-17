#pragma once
#include <core/core.hpp>
#include <physics/halfedgeedge.hpp>
#include <application/application.hpp>
#include <core/math/math.hpp>

#include <core/logging/logging.hpp>
#include <physics/components/physics_component.hpp>
#include <physics/components/rigidbody.hpp>
#include <physics/cube_collider_params.hpp>
#include <physics/data/physics_manifold_precursor.h>
#include <physics/systems/physicssystem.hpp>
#include <physics/halfedgeface.hpp>
#include <physics/data/penetrationquery.h>

#include <core/compute/context.hpp>
#include <core/compute/kernel.hpp>
#include <rendering/debugrendering.hpp>
#include <physics/systems/physicssystem.hpp>

#include <physics/physics_statics.hpp>

#include <audio/audio.hpp>

using namespace legion;

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

enum class PhysicsUnitTestMode
{
    CollisionDetectionMode,
    CollisionResolution
};

struct player_move : public app::input_axis<player_move> {};
struct player_strive : public app::input_axis<player_strive> {};
struct player_fly : public app::input_axis<player_fly> {};
struct player_look_x : public app::input_axis<player_look_x> {};
struct player_look_y : public app::input_axis<player_look_y> {};

// Move and strive for the wireframe sphere (which holds the only audio source)
// For testing the movement of audio sources (Spatial audio/doppler)
struct sphere_move : public app::input_axis<sphere_move> {};
struct sphere_strive : public app::input_axis<sphere_strive> {};
// Gain and Pitch knob
struct gain_change : public app::input_axis<gain_change> {};
struct pitch_change : public app::input_axis<pitch_change> {};

struct exit_action : public app::input_action<exit_action> {};

struct fullscreen_action : public app::input_action<fullscreen_action> {};
struct escape_cursor_action : public app::input_action<escape_cursor_action> {};
struct vsync_action : public app::input_action<vsync_action> {};


struct physics_test_move : public app::input_axis<physics_test_move> {};

struct play_audio_source : public app::input_action<play_audio_source> {};
struct pause_audio_source : public app::input_action<pause_audio_source> {};
struct stop_audio_source : public app::input_action<stop_audio_source> {};
struct rewind_audio_source : public app::input_action<rewind_audio_source> {};


class TestSystem final : public System<TestSystem>
{
public:
    TestSystem()
    {
        app::WindowSystem::requestWindow(world_entity_id, math::ivec2(1360, 768), "LEGION Engine", "Legion Icon", nullptr, nullptr, 1); // Create the request for the main window.
    }

    ecs::entity_handle player;
    ecs::entity_handle sphere;

    std::vector< ecs::entity_handle > physicsUnitTestCD;
    std::vector< ecs::entity_handle > physicsUnitTestCR;

    virtual void setup()
    {
#pragma region OpenCL
        compute::Program prog = fs::view("assets://kernels/vadd_kernel.cl").load_as<compute::Program>();

        prog.prewarm("vector_add");

        std::vector<int> ints;

        auto res = fs::view("assets://bigint.txt").get();
        if (res == common::valid) {

            char* buf = new char[6];
            memset(buf, 0, 6);
            filesystem::basic_resource contents = res;

            for (size_t i = 0; i < contents.size() && i < 5 * 2048; i += 5)
            {
                memcpy(buf, contents.data() + i, 5);
                ints.push_back(std::atol(buf));
            }

            delete[] buf;
        }

        std::vector<int> first_ints(ints.begin(), ints.begin() + ints.size() / 2);
        std::vector<int> second_ints(ints.begin() + ints.size() / 2, ints.end());

        size_t to_process = std::min(first_ints.size(), second_ints.size());

        std::vector<int> results(to_process);


        auto A = compute::Context::createBuffer(first_ints, compute::buffer_type::READ_BUFFER, "A");
        auto B = compute::Context::createBuffer(second_ints, compute::buffer_type::READ_BUFFER, "B");
        auto C = compute::Context::createBuffer(results, compute::buffer_type::WRITE_BUFFER, "C");

        prog.kernelContext("vector_add")
            .set_and_enqueue_buffer(A)
            .set_and_enqueue_buffer(B)
            .set_buffer(C)
            .global(1024)
            .local(64)
            .dispatch()
            .enqueue_buffer(C)
            .finish();

        /* for (int& i : results)
         {
             log::info("got {}", i);
         }*/
#pragma endregion

#pragma region Input binding
        app::InputSystem::createBinding<physics_test_move>(app::inputmap::method::LEFT, -1.f);
        app::InputSystem::createBinding<physics_test_move>(app::inputmap::method::RIGHT, 1.f);

        app::InputSystem::createBinding<player_move>(app::inputmap::method::W, 1.f);
        app::InputSystem::createBinding<player_move>(app::inputmap::method::S, -1.f);
        app::InputSystem::createBinding<player_strive>(app::inputmap::method::D, 1.f);
        app::InputSystem::createBinding<player_strive>(app::inputmap::method::A, -1.f);
        app::InputSystem::createBinding<player_fly>(app::inputmap::method::SPACE, 1.f);
        app::InputSystem::createBinding<player_fly>(app::inputmap::method::LEFT_SHIFT, -1.f);
        app::InputSystem::createBinding<player_look_x>(app::inputmap::method::MOUSE_X, 0.f);
        app::InputSystem::createBinding<player_look_y>(app::inputmap::method::MOUSE_Y, 0.f);
        app::InputSystem::createBinding<exit_action>(app::inputmap::method::ESCAPE);
        app::InputSystem::createBinding<fullscreen_action>(app::inputmap::method::F11);
        app::InputSystem::createBinding<escape_cursor_action>(app::inputmap::method::TAB);
        app::InputSystem::createBinding<vsync_action>(app::inputmap::method::F1);

        app::InputSystem::createBinding<sphere_move>(app::inputmap::method::I, 1.f);
        app::InputSystem::createBinding<sphere_move>(app::inputmap::method::K, -1.f);
        app::InputSystem::createBinding<sphere_strive>(app::inputmap::method::J, 1.f);
        app::InputSystem::createBinding<sphere_strive>(app::inputmap::method::U, 10.f); // Power mode of J
        app::InputSystem::createBinding<sphere_strive>(app::inputmap::method::L, -1.f);
        app::InputSystem::createBinding<sphere_strive>(app::inputmap::method::O, -10.f); // Power mode of O

        app::InputSystem::createBinding<gain_change>(app::inputmap::method::F10, 0.05f);
        app::InputSystem::createBinding<gain_change>(app::inputmap::method::F9, -0.05f);
        app::InputSystem::createBinding<pitch_change>(app::inputmap::method::APOSTROPHE, 1.0f);
        app::InputSystem::createBinding<pitch_change>(app::inputmap::method::SEMICOLON, -1.0f);

        app::InputSystem::createBinding<play_audio_source>(app::inputmap::method::P);
        app::InputSystem::createBinding<pause_audio_source>(app::inputmap::method::LEFT_BRACKET);
        app::InputSystem::createBinding<stop_audio_source>(app::inputmap::method::RIGHT_BRACKET);
        app::InputSystem::createBinding<rewind_audio_source>(app::inputmap::method::BACKSPACE);

        bindToEvent<player_move, &TestSystem::onPlayerMove>();
        bindToEvent<player_strive, &TestSystem::onPlayerStrive>();
        bindToEvent<player_fly, &TestSystem::onPlayerFly>();
        bindToEvent<player_look_x, &TestSystem::onPlayerLookX>();
        bindToEvent<player_look_y, &TestSystem::onPlayerLookY>();
        bindToEvent<exit_action, &TestSystem::onExit>();
        bindToEvent<fullscreen_action, &TestSystem::onFullscreen>();
        bindToEvent<escape_cursor_action, &TestSystem::onEscapeCursor>();
        bindToEvent<vsync_action, &TestSystem::onVSYNCSwap>();

        bindToEvent<physics_test_move, &TestSystem::onUnitPhysicsUnitTestMove>();

        bindToEvent<sphere_move, &TestSystem::onSphereAAMove>();
        bindToEvent<sphere_strive, &TestSystem::onSphereAAStrive>();
        bindToEvent<gain_change, &TestSystem::onGainChange>();
        bindToEvent<pitch_change, &TestSystem::onPitchChange>();

        bindToEvent<play_audio_source, &TestSystem::playAudioSource>();
        bindToEvent<pause_audio_source, &TestSystem::pauseAudioSource>();
        bindToEvent<stop_audio_source, &TestSystem::stopAudioSource>();
        bindToEvent<rewind_audio_source, &TestSystem::rewindAudioSource>();
#pragma endregion

        app::window window = m_ecs->world.get_component_handle<app::window>().read();
        window.enableCursor(false);
        window.show();

#pragma region Model and material loading
        rendering::model_handle cubeH;
        rendering::model_handle sphereH;
        rendering::model_handle suzanneH;
        //rendering::model_handle gnomeH;
        rendering::model_handle uvsphereH;
        rendering::model_handle axesH;
        rendering::model_handle submeshtestH;
        rendering::model_handle floorH;

        rendering::material_handle wireframeH;
        rendering::material_handle vertexH;

        rendering::material_handle uvH;
        rendering::material_handle normalH;
        rendering::material_handle skyboxH;
        rendering::material_handle floorMH;

        {
            async::readwrite_guard guard(*window.lock);
            app::ContextHelper::makeContextCurrent(window);

            cubeH = rendering::ModelCache::create_model("cube", "assets://models/cube.obj"_view);
            sphereH = rendering::ModelCache::create_model("sphere", "assets://models/sphere.obj"_view);
            suzanneH = rendering::ModelCache::create_model("suzanne", "assets://models/suzanne.obj"_view);
            //gnomeH = rendering::ModelCache::create_model("gnome", "assets://models/wizardgnome.obj"_view);
            uvsphereH = rendering::ModelCache::create_model("uvsphere", "assets://models/uvsphere.obj"_view);
            axesH = rendering::ModelCache::create_model("axes", "assets://models/xyz.obj"_view, { true, false, "assets://models/xyz.mtl"_view });
            submeshtestH = rendering::ModelCache::create_model("submeshtest", "assets://models/submeshtest.obj"_view);
            floorH = rendering::ModelCache::create_model("floor", "assets://models/groundplane.obj"_view);

            wireframeH = rendering::MaterialCache::create_material("wireframe", "assets://shaders/wireframe.glsl"_view);
            vertexH = rendering::MaterialCache::create_material("vertex", "assets://shaders/position.glsl"_view);
            uvH = rendering::MaterialCache::create_material("uv", "assets://shaders/uv.glsl"_view);
            normalH = rendering::MaterialCache::create_material("normal", "assets://shaders/normal.glsl"_view);
            skyboxH = rendering::MaterialCache::create_material("skybox", "assets://shaders/skybox.glsl"_view);
            floorMH = rendering::MaterialCache::create_material("floor", "assets://shaders/groundplane.glsl"_view);

            app::ContextHelper::makeContextCurrent(nullptr);
        }
#pragma endregion

#pragma region Entities
        {
            auto ent = createEntity();
            ent.add_component<rendering::renderable>({ cubeH, skyboxH });
            ent.add_components<transform>(position(), rotation(), scale(500.f));
        }

        {
            auto ent = createEntity();
            ent.add_component<rendering::renderable>({ floorH, floorMH });
            ent.add_components<transform>();
        }

        {
            auto ent = createEntity();
            ent.add_components<rendering::renderable, sah>({ suzanneH, vertexH }, {});
            ent.add_components<transform>(position(0, 3, 5.1f), rotation(), scale());

            auto [positionH, rotationH, scaleH] = ent.get_component_handles<transform>();

            log::trace("p {}, r {}, s {}, has {}", positionH.read(), rotationH.read(), scaleH.read(), ent.has_components<transform>());

            ent.remove_components<transform>();

            log::trace("p {}, r {}, s {}, has {}", positionH.read(), rotationH.read(), scaleH.read(), ent.has_components<position, rotation, scale>());
            transform transf = ent.add_components<transform>(position(0, 3, 5.1f), rotation(), scale());

            auto& [_, rotationH2, scaleH2] = transf.handles;
            auto positionH2 = transf.get<position>();

            log::trace("p {}, r {}, s {}, has {}", positionH2.read(), rotationH2.read(), scaleH2.read(), ent.has_components<position, rotation, scale>());
        }

        {
            auto ent = createEntity();
            ent.add_components<rendering::renderable, sah>({ suzanneH, wireframeH }, {});
            ent.add_components<transform>(position(0, 3, 8.1f), rotation(), scale());
        }

        {
            auto ent = m_ecs->createEntity();
            ent.add_components<rendering::renderable, sah>({ suzanneH, normalH }, {});

            ent.add_components<transform>(position(0, 3, 11.1f), rotation(), scale());
        }

        /*   {
               auto ent = m_ecs->createEntity();
               ent.add_component<sah>();
               m_ecs->createComponent<rendering::renderable>(ent, { gnomeH, normalH });

               auto [positionH, rotationH, scaleH] = m_ecs->createComponent<transform>(ent);
               positionH.write(math::vec3(0, 2, 15.1f));
               scaleH.write(math::vec3(1.f));
           }*/

        {
            auto ent = createEntity();
            ent.add_components<rendering::renderable, sah>({ submeshtestH, normalH }, {});
            ent.add_components<transform>(position(0, 10, 0), rotation(), scale());
        }

        {
            auto ent = createEntity();
            ent.add_component<rendering::renderable>({ axesH, normalH });
            ent.add_components<transform>();
        }

        {
            auto ent = createEntity();
            ent.add_components<rendering::renderable, sah>({ cubeH, uvH }, {});
            ent.add_components<transform>(position(5.1f, 3, 0), rotation(), scale(0.75f));
        }

        {
            auto ent = createEntity();
            ent.add_components<rendering::renderable, sah>({ sphereH, normalH }, {});
            ent.add_components<transform>(position(0, 3, -5.1f), rotation(), scale(2.5f));
        }

        // Sphere setup (with audio source)
        {
            sphere = createEntity();
            sphere.add_components<rendering::renderable, sah>({ uvsphereH, wireframeH }, {});
            sphere.add_components<transform>(position(-5.1f, 3, 0), rotation(), scale(2.5f));

            auto segment = audio::AudioSegmentCache::createAudioSegment("waterfall", "assets://audio/365921__inspectorj__waterfall-small-b[mono].mp3"_view);
            if (segment)
            {
                audio::audio_source source;
                source.setAudioHandle(segment);
                sphere.add_component<audio::audio_source>(source);
            }

        }
#pragma endregion

        setupCameraEntity();


        //---------------------------------------------------------- Physics Collision Unit Test -------------------------------------------------------------------//

        setupPhysicsCDUnitTest(cubeH, wireframeH);

        //----------- Rigidbody-Collider AABB Test------------//

        setupPhysicsCRUnitTest(cubeH, wireframeH);


        createProcess<&TestSystem::update>("Update");
        createProcess<&TestSystem::differentThread>("TestChain");
        createProcess<&TestSystem::differentInterval>("TestChain", 1.f);
        createProcess<&TestSystem::drawInterval>("Physics");

    }

    void setupPhysicsCDUnitTest(rendering::model_handle cubeH, rendering::material_handle wireframeH)
    {
        physics::cube_collider_params cubeParams;
        cubeParams.breadth = 2.0f;
        cubeParams.width = 2.0f;
        cubeParams.height = 2.0f;

        physics::cube_collider_params cubeParams2;
        cubeParams2.breadth = 3.0f;
        cubeParams2.width = 3.0f;
        cubeParams2.height = 3.0f;

        //----------- AABB to AABB Test(Single Axis)  ------------//
        //**
        {
            auto ent = m_ecs->createEntity();

            auto entPhyHande = ent.add_component<physics::physicsComponent>();

            physics::physicsComponent physicsComponent2;
            physics::physicsComponent::init(physicsComponent2);


            physicsComponent2.AddBox(cubeParams);
            physicsComponent2.isTrigger = false;
            entPhyHande.write(physicsComponent2);

            auto renderableHandle = m_ecs->createComponent<rendering::renderable>(ent);
            renderableHandle.write({ cubeH, wireframeH });

            auto [positionH, rotationH, scaleH] = m_ecs->createComponents<transform>(ent);
            positionH.write(math::vec3(0, -3.0f, 8.0f));
            scaleH.write(math::vec3(1.0f));
        }

        {
            auto ent = m_ecs->createEntity();
            physicsUnitTestCD.push_back(ent);
            auto entPhyHande = ent.add_component<physics::physicsComponent>();

            physics::physicsComponent physicsComponent2;
            physics::physicsComponent::init(physicsComponent2);


            physicsComponent2.AddBox(cubeParams);
            physicsComponent2.isTrigger = true;
            entPhyHande.write(physicsComponent2);


            auto renderableHandle = m_ecs->createComponent<rendering::renderable>(ent);
            renderableHandle.write({ cubeH, wireframeH });

            auto [positionH, rotationH, scaleH] = m_ecs->createComponents<transform>(ent);
            positionH.write(math::vec3(3.0, -3.0f, 8.0f));
            scaleH.write(math::vec3(1.0f));
        }



        //----------- AABB to AABB Test  ------------//
        //**
        {
            auto ent = m_ecs->createEntity();

            auto entPhyHande = ent.add_component<physics::physicsComponent>();

            physics::physicsComponent physicsComponent2;
            physics::physicsComponent::init(physicsComponent2);


            physicsComponent2.AddBox(cubeParams);
            physicsComponent2.isTrigger = false;
            entPhyHande.write(physicsComponent2);


            auto renderableHandle = m_ecs->createComponent<rendering::renderable>(ent);
            renderableHandle.write({ cubeH, wireframeH });

            auto [positionH, rotationH, scaleH] = m_ecs->createComponents<transform>(ent);
            positionH.write(math::vec3(0, -3.0f, 5.0f));
            scaleH.write(math::vec3(1.0f));
        }

        {
            auto ent = m_ecs->createEntity();
            physicsUnitTestCD.push_back(ent);
            auto entPhyHande = ent.add_component<physics::physicsComponent>();

            physics::physicsComponent physicsComponent2;
            physics::physicsComponent::init(physicsComponent2);


            physicsComponent2.AddBox(cubeParams);
            physicsComponent2.isTrigger = true;
            entPhyHande.write(physicsComponent2);


            auto renderableHandle = m_ecs->createComponent<rendering::renderable>(ent);
            renderableHandle.write({ cubeH, wireframeH });

            auto [positionH, rotationH, scaleH] = m_ecs->createComponents<transform>(ent);
            positionH.write(math::vec3(3.0, -2.0f, 4.0f));
            scaleH.write(math::vec3(1.0f));
        }
        //*/

        //----------- AABB to OBB Test  ------------//
        //**
        {
            auto ent = m_ecs->createEntity();

            auto entPhyHande = ent.add_component<physics::physicsComponent>();

            physics::physicsComponent physicsComponent2;
            physics::physicsComponent::init(physicsComponent2);


            physicsComponent2.AddBox(cubeParams);
            physicsComponent2.isTrigger = false;
            entPhyHande.write(physicsComponent2);


            auto renderableHandle = m_ecs->createComponent<rendering::renderable>(ent);
            renderableHandle.write({ cubeH, wireframeH });

            auto [positionH, rotationH, scaleH] = m_ecs->createComponents<transform>(ent);
            positionH.write(math::vec3(0, -3.0f, -2.0f));
            scaleH.write(math::vec3(1.0f));
        }

        {
            auto ent = m_ecs->createEntity();
            physicsUnitTestCD.push_back(ent);
            auto entPhyHande = ent.add_component<physics::physicsComponent>();

            physics::physicsComponent physicsComponent2;
            physics::physicsComponent::init(physicsComponent2);


            physicsComponent2.AddBox(cubeParams);
            physicsComponent2.isTrigger = true;
            entPhyHande.write(physicsComponent2);


            auto renderableHandle = m_ecs->createComponent<rendering::renderable>(ent);
            renderableHandle.write({ cubeH, wireframeH });

            auto [positionH, rotationH, scaleH] = m_ecs->createComponents<transform>(ent);
            positionH.write(math::vec3(1.5, -3.0f, -2.0f));

            auto rot = rotationH.read();
            rot *= math::angleAxis(45.f, math::vec3(0, 1, 0));
            rot *= math::angleAxis(45.f, math::vec3(0, 0, 1));

            rotationH.write(rot);

            scaleH.write(math::vec3(1.0f));
        }
        //*/
        //----------- OBB to OBB Test  ------------//
        //**
        {
            auto ent = m_ecs->createEntity();

            auto entPhyHande = ent.add_component<physics::physicsComponent>();

            physics::physicsComponent physicsComponent2;
            physics::physicsComponent::init(physicsComponent2);


            physicsComponent2.AddBox(cubeParams);
            physicsComponent2.isTrigger = false;
            entPhyHande.write(physicsComponent2);


            auto renderableHandle = m_ecs->createComponent<rendering::renderable>(ent);
            renderableHandle.write({ cubeH, wireframeH });

            auto [positionH, rotationH, scaleH] = m_ecs->createComponents<transform>(ent);
            positionH.write(math::vec3(0, -3.0f, -7.0f));

            auto rot = rotationH.read();
            rot *= math::angleAxis(20.f, math::vec3(0, 1, 0));
            rotationH.write(rot);

            scaleH.write(math::vec3(1.0f));
        }

        {
            auto ent = m_ecs->createEntity();
            physicsUnitTestCD.push_back(ent);
            auto entPhyHande = ent.add_component<physics::physicsComponent>();

            physics::physicsComponent physicsComponent2;
            physics::physicsComponent::init(physicsComponent2);


            physicsComponent2.AddBox(cubeParams);
            physicsComponent2.isTrigger = true;
            entPhyHande.write(physicsComponent2);

            auto renderableHandle = m_ecs->createComponent<rendering::renderable>(ent);
            renderableHandle.write({ cubeH, wireframeH });

            auto [positionH, rotationH, scaleH] = m_ecs->createComponents<transform>(ent);
            positionH.write(math::vec3(3.0, -3.0f, -7.0f));

            auto rot = rotationH.read();
            rot *= math::angleAxis(45.f, math::vec3(0, 1, 0));
            rot *= math::angleAxis(45.f, math::vec3(0, 0, 1));

            rotationH.write(rot);

            scaleH.write(math::vec3(1.0f));
        }
        //*/

        //----------- OBB Edge-Edge Test  ------------//
        //**
        {
            auto ent = m_ecs->createEntity();
            //physicsUnitTestObjects.push_back(ent);
            auto entPhyHande = ent.add_component<physics::physicsComponent>();

            physics::physicsComponent physicsComponent2;
            physics::physicsComponent::init(physicsComponent2);


            physicsComponent2.AddBox(cubeParams);
            physicsComponent2.isTrigger = false;
            entPhyHande.write(physicsComponent2);


            auto renderableHandle = m_ecs->createComponent<rendering::renderable>(ent);
            renderableHandle.write({ cubeH, wireframeH });

            auto [positionH, rotationH, scaleH] = m_ecs->createComponents<transform>(ent);
            positionH.write(math::vec3(0, -3.0f, -12.0f));

            auto rot = rotationH.read();
            rot *= math::angleAxis(45.f, math::vec3(0, 1, 0));
            rotationH.write(rot);

            scaleH.write(math::vec3(1.0f));
        }
        {
            auto ent = m_ecs->createEntity();
            physicsUnitTestCD.push_back(ent);
            auto entPhyHande = ent.add_component<physics::physicsComponent>();

            physics::physicsComponent physicsComponent2;
            physics::physicsComponent::init(physicsComponent2);


            physicsComponent2.AddBox(cubeParams);
            physicsComponent2.isTrigger = true;
            entPhyHande.write(physicsComponent2);


            auto renderableHandle = m_ecs->createComponent<rendering::renderable>(ent);
            renderableHandle.write({ cubeH, wireframeH });

            auto [positionH, rotationH, scaleH] = m_ecs->createComponents<transform>(ent);
            positionH.write(math::vec3(3.0, -3.0f, -13.0f));

            auto rot = rotationH.read();
            rot *= math::angleAxis(45.f, math::vec3(1, 0, 0));
            rot *= math::angleAxis(45.f, math::vec3(0, 1, 0));
            rotationH.write(rot);

            scaleH.write(math::vec3(1.0f));
        }
        //*/
    }

    void setupPhysicsCRUnitTest(rendering::model_handle cubeH, rendering::material_handle wireframeH)
    {

    }

    void setupCameraEntity()
    {
        player = createEntity();
        rotation rot = math::conjugate(math::normalize(math::toQuat(math::lookAt(math::vec3(0, 0, 0), math::vec3(0, 0, 1), math::vec3(0, 1, 0)))));
        player.add_components<transform>(position(0.f, 3.f, 0.f), rot, scale());
        player.add_component<audio::audio_listener>();

        rendering::camera cam;
        cam.set_projection(90.f, 0.1f, 1000.f);
        player.add_component<rendering::camera>(cam);
    }

    void onExit(exit_action* action)
    {
        if (action->released())
            raiseEvent<events::exit>();
    }

    void onFullscreen(fullscreen_action* action)
    {
        if (action->released())
        {
            app::WindowSystem::requestFullscreenToggle(world_entity_id, math::ivec2(100, 100), math::ivec2(1360, 768));
        }
    }

    void onEscapeCursor(escape_cursor_action* action)
    {
        if (action->released())
        {
            static bool enabled = false;
            app::window window = m_ecs->world.get_component_handle<app::window>().read();
            enabled = !enabled;
            window.enableCursor(enabled);
            window.show();
        }
    }

    void onVSYNCSwap(vsync_action* action)
    {
        if (action->released())
        {
            auto handle = m_ecs->world.get_component_handle<app::window>();
            app::window window = handle.read();
            window.setSwapInterval(window.swapInterval() ? 0 : 1);
            log::debug("set swap interval to {}", window.swapInterval());
            handle.write(window);
        }
    }

    void onPlayerMove(player_move* action)
    {
        auto posH = player.get_component_handle<position>();
        auto rot = player.get_component_handle<rotation>().read();
        math::vec3 move = math::toMat3(rot) * math::vec3(0.f, 0.f, 1.f);
        move = math::normalize(move * math::vec3(1, 0, 1)) * action->value * action->input_delta * 10.f;
        posH.fetch_add(move);
    }

    void onPlayerStrive(player_strive* action)
    {
        auto posH = player.get_component_handle<position>();
        auto rot = player.get_component_handle<rotation>().read();
        math::vec3 move = math::toMat3(rot) * math::vec3(1.f, 0.f, 0.f);
        move = math::normalize(move * math::vec3(1, 0, 1)) * action->value * action->input_delta * 10.f;
        posH.fetch_add(move);
    }

    void onSphereAAMove(sphere_move* action)
    {
        auto posH = sphere.get_component_handle<position>();
        math::vec3 move = math::vec3(0.f, 0.f, 1.f);
        move = math::normalize(move * math::vec3(1, 0, 1)) * action->value * action->input_delta * 10.f;
        posH.fetch_add(move);
    }

    void onSphereAAStrive(sphere_strive* action)
    {
        auto posH = sphere.get_component_handle<position>();
        math::vec3 move = math::vec3(1.f, 0.f, 0.f);
        move = math::normalize(move * math::vec3(1, 0, 1)) * action->value * action->input_delta * 10.f;
        posH.fetch_add(move);
    }

    void onGainChange(gain_change* action)
    {
        if (action->value == 0) return;
        using namespace audio;
        auto sourceH = sphere.get_component_handle<audio_source>();
        audio_source source = sourceH.read();
        const float g = source.getGain() + action->value * action->input_delta * 10.0f;
        source.setGain(g);
        sourceH.write(source);
    }

    void onPitchChange(pitch_change* action)
    {
        if (action->value == 0) return;
        using namespace audio;
        auto sourceH = sphere.get_component_handle<audio_source>();
        audio_source source = sourceH.read();
        const float p = source.getPitch() + action->value * action->input_delta * 10.0f;
        source.setPitch(p);
        sourceH.write(source);
    }

    void onPlayerFly(player_fly* action)
    {
        auto posH = player.get_component_handle<position>();
        posH.fetch_add(math::vec3(0.f, action->value * action->input_delta * 10.f, 0.f));
    }

    void onPlayerLookX(player_look_x* action)
    {
        auto rotH = player.get_component_handle<rotation>();
        rotH.fetch_multiply(math::angleAxis(action->value, math::vec3(0, 1, 0)));
        rotH.read_modify_write(rotation(), [](const rotation& src, rotation&& dummy)
            {
                (void)dummy;
                math::vec3 fwd = math::toMat3(src) * math::vec3(0.f, 0.f, 1.f);
                if (fwd.y < -0.95f)
                    fwd.y = -0.95f;
                else if (fwd.y > 0.95f)
                    fwd.y = 0.95f;
                fwd = math::normalize(fwd);
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
                if (fwd.y < -0.95f)
                    fwd.y = -0.95f;
                else if (fwd.y > 0.95f)
                    fwd.y = 0.95f;
                fwd = math::normalize(fwd);
                math::vec3 right = math::cross(fwd, math::vec3(0.f, 1.f, 0.f));
                return (rotation)math::conjugate(math::toQuat(math::lookAt(math::vec3(0.f, 0.f, 0.f), fwd, math::cross(right, fwd))));
            });
    }

    void onUnitPhysicsUnitTestMove(physics_test_move* action)
    {
        for (auto entity : physicsUnitTestCD)
        {
            auto posHandle = entity.get_component_handle<position>();

            auto pos = posHandle.read();
            pos.x += 0.005 * action->value;


            posHandle.write(pos);

        }
    }

    void playAudioSource(play_audio_source* action)
    {
        auto sourceH = sphere.get_component_handle<audio::audio_source>();
        audio::audio_source source = sourceH.read();
        source.play();
        sourceH.write(source);
    }

    void pauseAudioSource(pause_audio_source* action)
    {
        auto sourceH = sphere.get_component_handle<audio::audio_source>();
        audio::audio_source source = sourceH.read();
        source.pause();
        sourceH.write(source);
    }

    void stopAudioSource(stop_audio_source* action)
    {
        auto sourceH = sphere.get_component_handle<audio::audio_source>();
        audio::audio_source source = sourceH.read();
        source.stop();
        sourceH.write(source);
    }

    void rewindAudioSource(rewind_audio_source* action)
    {
        auto sourceH = sphere.get_component_handle<audio::audio_source>();
        audio::audio_source source = sourceH.read();
        source.rewind();
        sourceH.write(source);
    }

    void update(time::span deltaTime)
    {
        debug::drawLine(math::vec3(0, 0, 0), math::vec3(1, 0, 0), math::colors::red, 10);
        debug::drawLine(math::vec3(0, 0, 0), math::vec3(0, 1, 0), math::colors::green, 10);
        debug::drawLine(math::vec3(0, 0, 0), math::vec3(0, 0, 1), math::colors::blue, 10);

        //log::info("still alive! {}",deltaTime.seconds());
        static auto sahQuery = createQuery<sah, rotation, position>();

        //static time::span buffer;
        static int frameCount;
        //static time::span accumulated;

        //buffer += deltaTime;
        //accumulated += deltaTime;
        frameCount++;

        for (auto entity : sahQuery)
        {
            auto comp = entity.get_component_handle<sah>();

            auto rot = entity.read_component<rotation>();

            rot *= math::angleAxis(math::deg2rad(45.f * deltaTime), math::vec3(0, 1, 0));

            entity.write_component(rot);

            comp.write({ frameCount });

            auto pos = entity.read_component<position>();

            debug::drawLine(pos, pos + rot.forward(), math::colors::magenta, 10);
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
         //	//throw legion_exception_msg("hehehe fuck you >:D");
         //}
    }

    void drawInterval(time::span deltaTime)
    {
        static auto physicsQuery = createQuery< physics::physicsComponent>();
        uint i = 0;


        for (auto penetration : physics::PhysicsSystem::penetrationQueries)
        {
            debug::drawLine(penetration->faceCentroid
                , penetration->faceCentroid + penetration->normal, math::vec4(0, 0.5f, 0, 1), 15.0f);

        }

        //--------------------------------------- Draw contact points ---------------------------------------//
        i = 0;
        for (auto contact : physics::PhysicsSystem::contactPoints)
        {
            debug::drawLine(contact.worldContactInc
                , contact.worldContactRef, math::vec4(1, 0, 0, 1), 5.0f);

            debug::drawLine(contact.worldContactInc
                , contact.worldContactInc + math::vec3(0, 0.1f, 0), math::vec4(0.5, 0.5, 0.5, 1), 5.0f, 0, true);

            debug::drawLine(contact.worldContactRef
                , contact.worldContactRef + math::vec3(0, 0.1f, 0), math::vec4(0, 0, 0, 1), 5.0f, 0, true);

        }

        //--------------------------------------- Draw extreme points ---------------------------------------//
        i = 0;
        for (auto penetration : physics::PhysicsSystem::aPoint)
        {
            debug::drawLine(penetration
                , penetration + math::vec3(0, 0.2, 0), math::vec4(1, 0, 0, 1), 15.0f);

        }
        i = 0;
        for (auto penetration : physics::PhysicsSystem::bPoint)
        {
            debug::drawLine(penetration
                , penetration + math::vec3(0, 0.2, 0), math::vec4(0, 0, 1, 1), 15.0f);

        }

        physics::PhysicsSystem::penetrationQueries.clear();
        physics::PhysicsSystem::aPoint.clear();
        physics::PhysicsSystem::bPoint.clear();
        physics::PhysicsSystem::contactPoints.clear();

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

                i = 0;
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

                        //log::debug(("---- face"));
                        do
                        {
                            //log::debug(("edge"));
                            physics::HalfEdgeEdge* edgeToExecuteOn = currentEdge;
                            currentEdge = currentEdge->nextEdge;

                            math::vec3 worldStart = localTransform * math::vec4(*(edgeToExecuteOn->edgePositionPtr), 1);
                            math::vec3 worldEnd = localTransform * math::vec4(*(edgeToExecuteOn->nextEdge->edgePositionPtr), 1);

                            debug::drawLine(worldStart, worldEnd, math::colors::green, 5.0f);
                        } while (initialEdge != currentEdge && currentEdge != nullptr);
                    }


                    physCollider->collisionsFound.clear();
                }

            }

        }

        //FindClosestPointsToLineSegment unit test


        math::vec3 p1(5, -0.5, 0);
        math::vec3 p2(5, 0.5, 0);

        math::vec3 p3(6, 0, -0.5);
        math::vec3 p4(6, 0, 0.5);

        math::vec3 p1p2;
        math::vec3 p3p4;

        debug::drawLine(p1, p2, math::colors::red, 5.0f);
        debug::drawLine(p3, p4, math::colors::red, 5.0f);

        physics::PhysicsStatics::FindClosestPointsToLineSegment(p1, p2, p3, p4, p1p2, p3p4);

        debug::drawLine(p1p2, p3p4, math::colors::green, 5.0f);

        p1 = math::vec3(8, 0, 0);
        p2 = p1 + math::vec3(0, 1.0f, 0);

        p3 = math::vec3(10, 0, 0) + math::vec3(1.0f);
        p4 = p3 - math::vec3(1.0f);

        debug::drawLine(p1, p2, math::colors::red, 5.0f);
        debug::drawLine(p3, p4, math::colors::red, 5.0f);

        physics::PhysicsStatics::FindClosestPointsToLineSegment(p1, p2, p3, p4, p1p2, p3p4);

        debug::drawLine(p1p2, p3p4, math::colors::green, 5.0f);

        //log::debug("{:.3f}", deltaTime);
    }
};
