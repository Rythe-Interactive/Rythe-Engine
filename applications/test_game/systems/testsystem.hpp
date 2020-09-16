#pragma once
#include <core/core.hpp>
#include <application/application.hpp>
#include <core/math/math.hpp>

#include <core/logging/logging.hpp>
#include <physics/physics_component.hpp>
#include <physics/rigidbody.hpp>
#include <physics/cube_collider_params.hpp>

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
        app::InputSystem::createBinding<player_look_x>(app::inputmap::method::MOUSE_X, 1.f);
        app::InputSystem::createBinding<player_look_y>(app::inputmap::method::MOUSE_Y, 1.f);
        app::InputSystem::createBinding<exit_action>(app::inputmap::method::ESCAPE);

        bindToEvent<player_move, &TestSystem::onPlayerMove>();
        bindToEvent<player_strive, &TestSystem::onPlayerStrive>();
        bindToEvent<player_fly, &TestSystem::onPlayerFly>();
        bindToEvent<player_look_x, &TestSystem::onPlayerLookX>();
        bindToEvent<player_look_y, &TestSystem::onPlayerLookY>();
        bindToEvent<exit_action, &TestSystem::onExit>();

        app::window window = m_ecs->world.get_component_handle<app::window>().read();
        window.enableCursor(false);
        rendering::model_handle modelH;
        rendering::material_handle wireframeH;
        rendering::material_handle vertexH;

        {
            async::readwrite_guard guard(*window.lock);
            app::ContextHelper::makeContextCurrent(window);

            modelH = rendering::ModelCache::create_model("test", "basic://models/Cube.obj"_view);
            wireframeH = rendering::MaterialCache::create_material("wireframe", "basic:/shaders/wireframe.glsl"_view);
            vertexH = rendering::MaterialCache::create_material("vertex", "basic:/shaders/position.glsl"_view);

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

        auto physicsEnt = m_ecs->createEntity();

        //setup rendering for physics ent
        m_ecs->createComponent<rendering::renderable>(physicsEnt, { modelH, wireframeH });

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
    }

    void onPlayerStrive(player_strive* action)
    {
        auto posH = player.get_component_handle<position>();
        auto rot = player.get_component_handle<rotation>().read();
        math::vec3 move = math::toMat3(rot) * math::vec3(1.f, 0.f, 0.f);
        move = math::normalize(move * math::vec3(1, 0, 1)) * action->value * action->input_delta * 6.f;
        posH.fetch_add(move);
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

    void update(time::span deltaTime)
    {

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
