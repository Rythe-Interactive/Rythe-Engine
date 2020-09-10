#pragma once
#include <core/core.hpp>
#include <application/application.hpp>
#include <core/math/math.hpp>

#include <core/logging/logging.hpp>
#include <physics/physics_component.hpp>
#include <physics/rigidbody.hpp>


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

struct player_move_action : public application::input_axis<player_move_action> {};

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

        application::InputSystem::createBinding<player_move_action>(application::inputmap::method::W, 1.f);
        application::InputSystem::createBinding<player_move_action>(application::inputmap::method::S, -1.f);
        bindToEvent<player_move_action, &TestSystem::onPlayerMove>();

        //------------------------------------- Setup rotating block entity-------------------------------------------//

        auto ent = m_ecs->createEntity();
        ent.add_component<sah>();
        auto renderableHandle = m_ecs->createComponent<rendering::renderable>(ent);

        rendering::renderable rendercomp = renderableHandle.read();

        fs::view meshFile("basic://models/Cube.obj");

        rendercomp.model = rendering::model_cache::create_model("test", meshFile);

        renderableHandle.write(rendercomp);

        auto [positionH, rotationH, scaleH] = m_ecs->createComponent<transform>(ent);

        position pos = positionH.read();
       
        rotation otherRot = rotationH.read();

        pos.z = 5.1f;

        positionH.write(pos);

        //std::cout << ent.has_component<position>() << std::endl;
        //std::cout << ent.has_component<rotation>() << std::endl;
        //std::cout << ent.has_component<scale>() << std::endl;

        //raiseEvent<application::window_request>(ent, math::ivec2(600, 300), "This is a test window!");

        setupCameraEntity();
        //raiseEvent<application::window_request>(player, math::ivec2(600, 300), "This is a test window2!");

        //------------------------------------- Setup entity with rigidbody -------------------------------------------//

        auto physicsEnt = m_ecs->createEntity();

        //setup rendering for physics ent
        auto renderableHandle2 = m_ecs->createComponent<rendering::renderable>(physicsEnt);

        rendering::renderable rendercomp2 = renderableHandle2.read();
        rendercomp2.model = rendering::model_cache::create_model("test", meshFile);
        renderableHandle2.write(rendercomp2);

        auto [bodyPosition, bodyRotation, bodyScale] = m_ecs->createComponent<transform>(physicsEnt);

        position bodyP = bodyPosition.read();
        bodyP.x = 0.0f;
        bodyP.y = 4.0f;
        bodyP.z = 5.1f;
        bodyPosition.write(bodyP);

        //setup physics component on physics ent
        auto rbHandle = m_ecs->createComponent<physics::rigidbody>(physicsEnt);
        auto physicsComponent = m_ecs->createComponent<physics::physicsComponent>(physicsEnt);

        auto rb = rbHandle.read();

        rb.globalCentreOfMass = bodyP;
        //rb.addForce( math::vec3(-9, 0, 0));
        rb.addForceAt(math::vec3(0, 4.5, 5.1f),math::vec3(-100, 0, 0));
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

        position camPos = camPosHandle.read();
        camPos.z = -20.0f;
        camPos.x = -8.0f;
        camPosHandle.write(camPos);


        rotation rot = camRotHandle.read();
        rot = math::toQuat(math::inverse(math::lookAtRH(math::vec3(0, 0, 0), math::vec3(0, 0, -1), math::vec3(0, -1, 0))));
        camRotHandle.write(rot);


        auto camH = m_ecs->createComponent<rendering::camera>(player);
        rendering::camera cam = camH.read();

        cam.set_projection(60.f, 1360.f / 768.f, 0.1);
        camH.write(cam);
    }

    void onPlayerMove(player_move_action* action)
    {
        auto posH = player.get_component_handle<position>();
        auto pos = posH.read();
        pos.z += action->value * 0.1f;
        std::cout << "[ " << pos.x << ", " << pos.y << ", " << pos.z << " ]\n";
        posH.write(pos);
    }

    void update(time::span deltaTime)
    {

        log::info("still alive! {}",deltaTime.seconds());
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
