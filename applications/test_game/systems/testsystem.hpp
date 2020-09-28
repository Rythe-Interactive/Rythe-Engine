#pragma once
#include <core/core.hpp>
#include <application/application.hpp>
#include <core/math/math.hpp>

#include <core/logging/logging.hpp>
#include <physics/physics_component.hpp>
#include <physics/rigidbody.hpp>
#include <physics/cube_collider_params.hpp>
#include <rendering/debugrendering.hpp>

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

struct player_move : public app::input_axis<player_move> {};
struct player_strive : public app::input_axis<player_strive> {};
struct player_fly : public app::input_axis<player_fly> {};
struct player_look_x : public app::input_axis<player_look_x> {};
struct player_look_y : public app::input_axis<player_look_y> {};

struct exit_action : public app::input_action<exit_action> {};
struct fullscreen_action : public app::input_action<fullscreen_action> {};
struct escape_cursor_action : public app::input_action<escape_cursor_action> {};
struct vsync_action : public app::input_action<vsync_action> {};

class TestSystem final : public System<TestSystem>
{
public:
    ecs::entity_handle player;

    virtual void setup()
    {
        filter(log::severity::debug);
        log::info("Hello World");
        log::warn("Hello World");
        log::error("Hello World");
        log::debug("Hello World");

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

        bindToEvent<player_move, &TestSystem::onPlayerMove>();
        bindToEvent<player_strive, &TestSystem::onPlayerStrive>();
        bindToEvent<player_fly, &TestSystem::onPlayerFly>();
        bindToEvent<player_look_x, &TestSystem::onPlayerLookX>();
        bindToEvent<player_look_y, &TestSystem::onPlayerLookY>();
        bindToEvent<exit_action, &TestSystem::onExit>();
        bindToEvent<fullscreen_action, &TestSystem::onFullscreen>();
        bindToEvent<escape_cursor_action, &TestSystem::onEscapeCursor>();
        bindToEvent<vsync_action, &TestSystem::onVSYNCSwap>();

        app::window window = m_ecs->world.get_component_handle<app::window>().read();
        window.enableCursor(false);
        window.show();
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

        {
            auto ent = m_ecs->createEntity();
            m_ecs->createComponent<rendering::renderable>(ent, { cubeH, skyboxH });

            auto [positionH, rotationH, scaleH] = m_ecs->createComponent<transform>(ent);
            scaleH.write(math::vec3(500.f));
        }

        {
            auto ent = m_ecs->createEntity();
            m_ecs->createComponent<rendering::renderable>(ent, { floorH, floorMH });

            m_ecs->createComponent<transform>(ent);
        }

        {
            auto ent = m_ecs->createEntity();
            ent.add_component<sah>();
            ent.add_component<rendering::renderable>({ suzanneH, vertexH });

            auto [positionH, rotationH, scaleH] = m_ecs->createComponent<transform>(ent);
            positionH.write(math::vec3(0, 3, 5.1f));
            scaleH.write(math::vec3(1.f));
        }

        {
            auto ent = m_ecs->createEntity();
            ent.add_component<sah>();
            m_ecs->createComponent<rendering::renderable>(ent, { suzanneH, wireframeH });

            auto [positionH, rotationH, scaleH] = m_ecs->createComponent<transform>(ent);
            positionH.write(math::vec3(0, 3, 8.1f));
            scaleH.write(math::vec3(1.f));
        }

        {
            auto ent = m_ecs->createEntity();
            ent.add_component<sah>();
            m_ecs->createComponent<rendering::renderable>(ent, { suzanneH, normalH });

            auto [positionH, rotationH, scaleH] = m_ecs->createComponent<transform>(ent);
            positionH.write(math::vec3(0, 3, 11.1f));
            scaleH.write(math::vec3(1.f));
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
            auto ent = m_ecs->createEntity();
            ent.add_component<sah>();
            m_ecs->createComponent<rendering::renderable>(ent, { submeshtestH, normalH });

            auto [positionH, rotationH, scaleH] = m_ecs->createComponent<transform>(ent);
            positionH.write(math::vec3(0, 10, 0));
            scaleH.write(math::vec3(1.f));
        }

        {
            auto ent = m_ecs->createEntity();
            m_ecs->createComponent<rendering::renderable>(ent, { axesH, normalH });
            m_ecs->createComponent<transform>(ent);
        }

        {
            auto ent = m_ecs->createEntity();
            ent.add_component<sah>();
            m_ecs->createComponent<rendering::renderable>(ent, { cubeH, uvH });

            auto [positionH, rotationH, scaleH] = m_ecs->createComponent<transform>(ent);
            positionH.write(math::vec3(5.1f, 3, 0));
            scaleH.write(math::vec3(0.75f));
        }

        {
            auto ent = m_ecs->createEntity();
            ent.add_component<sah>();
            m_ecs->createComponent<rendering::renderable>(ent, { sphereH, normalH });

            auto [positionH, rotationH, scaleH] = m_ecs->createComponent<transform>(ent);
            positionH.write(math::vec3(0, 3, -5.1f));
            scaleH.write(math::vec3(2.5f));
        }

        {
            auto ent = m_ecs->createEntity();
            ent.add_component<sah>();
            m_ecs->createComponent<rendering::renderable>(ent, { uvsphereH, wireframeH });

            auto [positionH, rotationH, scaleH] = m_ecs->createComponent<transform>(ent);
            positionH.write(math::vec3(-5.1f, 3, 0));
            scaleH.write(math::vec3(2.5f));
        }

        setupCameraEntity();

        //------------------------------------- Setup entity with rigidbody -------------------------------------------//

        auto physicsEnt = m_ecs->createEntity();

        //setup rendering for physics ent
        m_ecs->createComponent<rendering::renderable>(physicsEnt, { cubeH, wireframeH });

        auto [bodyPosition, bodyRotation, bodyScale] = m_ecs->createComponent<transform>(physicsEnt);

        position bodyP = bodyPosition.read();
        bodyP.x = 0.0f;
        bodyP.y = 4.0f;
        bodyP.z = 5.1f;
        bodyPosition.write(bodyP);

        //setup physics component on physics ent
        auto rbHandle = m_ecs->createComponent<physics::rigidbody>(physicsEnt);
        auto physicsComponentHandle = m_ecs->createComponent<physics::physicsComponent>(physicsEnt);

        physics::physicsComponent physicsComponent;
        physics::physicsComponent::init(physicsComponent);

        physics::cube_collider_params cubeParams;
        physicsComponent.AddBox(cubeParams);

        physicsComponentHandle.write(physicsComponent);

        auto rb = rbHandle.read();

        rb.globalCentreOfMass = bodyP;
        //rb.addForce( math::vec3(-9, 0, 0));
        rb.addForceAt(math::vec3(0, 4.5, 5.1f), math::vec3(-100, 0, 0));
        rb.globalCentreOfMass = bodyP;

        rbHandle.write(rb);

        {
            auto ent = m_ecs->createEntity();
            ent.add_component<physics::physicsComponent>();
            auto renderableHandle = m_ecs->createComponent<rendering::renderable>(ent);
            renderableHandle.write({ cubeH, wireframeH });

            auto [positionH, rotationH, scaleH] = m_ecs->createComponent<transform>(ent);
            positionH.write(math::vec3(5.1f, -2.0f, 0));
            scaleH.write(math::vec3(0.25f));
        }


        createProcess<&TestSystem::update>("Update");
        createProcess<&TestSystem::differentThread>("TestChain");
        createProcess<&TestSystem::differentInterval>("TestChain", 1.f);

    }

    void setupCameraEntity()
    {
        player = m_ecs->createEntity();
        auto [camPosHandle, camRotHandle, camScaleHandle] = m_ecs->createComponent<transform>(player);

        rotation rot = camRotHandle.read();
        rot = math::conjugate(math::normalize(math::toQuat(math::lookAt(math::vec3(0, 0, 0), math::vec3(0, 0, 1), math::vec3(0, 1, 0)))));
        camRotHandle.write(rot);

        camPosHandle.write({ 0.f, 3.f, 0.f });

        auto camH = m_ecs->createComponent<rendering::camera>(player);
        rendering::camera cam = camH.read();

        cam.set_projection(90.f, 0.1f, 1000.f);
        camH.write(cam);
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
            raiseEvent<app::window_fullscreen_request>(world_entity_id, math::ivec2(100, 100), math::ivec2(1360, 768));
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
            window.setSwapInterval(window.swapInterval ? 0 : 1);
            log::debug("set swap interval to {}", window.swapInterval);
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

    void update(time::span deltaTime)
    {
        debug::drawLine(math::vec3(0, 0, 0), math::vec3(1, 0, 0), math::colors::red, 10);
        debug::drawLine(math::vec3(0, 0, 0), math::vec3(0, 1, 0), math::colors::green, 10);
        debug::drawLine(math::vec3(0, 0, 0), math::vec3(0, 0, 1), math::colors::blue, 10);

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
