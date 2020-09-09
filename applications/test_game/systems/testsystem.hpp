#pragma once
#include <core/core.hpp>
#include <application/application.hpp>
#include <core/math/math.hpp>

#include "core/logging/logging.hpp"
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

struct player_move : public application::input_axis<player_move> {};
struct player_strive : public application::input_axis<player_strive> {};
struct player_fly : public application::input_axis<player_fly> {};

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

        application::InputSystem::createBinding<player_move>(application::inputmap::method::W, 1.f);
        application::InputSystem::createBinding<player_move>(application::inputmap::method::S, -1.f);
        application::InputSystem::createBinding<player_strive>(application::inputmap::method::D, 1.f);
        application::InputSystem::createBinding<player_strive>(application::inputmap::method::A, -1.f);
        application::InputSystem::createBinding<player_fly>(application::inputmap::method::SPACE, 1.f);
        application::InputSystem::createBinding<player_fly>(application::inputmap::method::LEFT_SHIFT, -1.f);


        bindToEvent<player_move, &TestSystem::onPlayerMove>();
        bindToEvent<player_strive, &TestSystem::onPlayerStrive>();
        bindToEvent<player_fly, &TestSystem::onPlayerFly>();

        auto ent = m_ecs->createEntity();
        ent.add_component<sah>();
        auto renderableHandle = m_ecs->createComponent<rendering::renderable>(ent);

        rendering::renderable rendercomp = renderableHandle.read();

        fs::view meshFile("basic://models/Cube.obj");

        rendercomp.model = rendering::model_cache::create_model("test", meshFile);

        renderableHandle.write(rendercomp);

        auto [positionH, rotationH, scaleH] = m_ecs->createComponent<transform>(ent);

        position pos = positionH.read();

        pos.z = 5.1f;

        positionH.write(pos);

        //std::cout << ent.has_component<position>() << std::endl;
        //std::cout << ent.has_component<rotation>() << std::endl;
        //std::cout << ent.has_component<scale>() << std::endl;

        //raiseEvent<application::window_request>(ent, math::ivec2(600, 300), "This is a test window!");

        player = m_ecs->createEntity();
        auto [posCam, rotCam, scaleCam] = m_ecs->createComponent<transform>(player);

        rotation rot = rotCam.read();
        rot = math::toQuat(math::lookAt(math::vec3(0, 0, 0), math::vec3(0, 0, 1), math::vec3(0, 1, 0)));
        rotCam.write(rot);


        auto camH = m_ecs->createComponent<rendering::camera>(player);
        rendering::camera cam = camH.read();

        cam.set_projection(60.f, 1360.f / 768.f, 0.1, 1000.f);
        camH.write(cam);

        //raiseEvent<application::window_request>(player, math::ivec2(600, 300), "This is a test window2!");

        createProcess<&TestSystem::update>("Update");
        createProcess<&TestSystem::differentThread>("TestChain");
        createProcess<&TestSystem::differentInterval>("TestChain", 1.f);
    }

    void onPlayerMove(player_move* action)
    {
        auto posH = player.get_component_handle<position>();
        log::debug("z");
        posH.fetch_add(math::vec3(0.f, 0.f, action->value * 0.1f));
    }

    void onPlayerStrive(player_strive* action)
    {
        auto posH = player.get_component_handle<position>();
        log::debug("x");
        posH.fetch_add(math::vec3(action->value * 0.1f, 0.f, 0.f));
    }

    void onPlayerFly(player_fly* action)
    {
        auto posH = player.get_component_handle<position>();
        log::debug("y");
        posH.fetch_add(math::vec3(0.f, action->value * 0.1f, 0.f));
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
