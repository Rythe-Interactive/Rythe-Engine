#pragma once
#include <core/core.hpp>
#include <application/application.hpp>
#include <core/math/math.hpp>
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
    virtual void setup()
    {
    
        application::InputSystem::createBinding<player_move_action>(application::inputmap::method::A,-1);
        auto ent = m_ecs->createEntity();
        ent.add_component<sah>();

        raiseEvent<application::window_request>(ent, math::ivec2(600, 300), "This is a test window!");

        auto ent2 = m_ecs->createEntity();
        raiseEvent<application::window_request>(ent2, math::ivec2(600, 300), "This is a test window2!");
        
        bindToEvent<player_move_action,&TestSystem::onPlayerMove>();
        createProcess<&TestSystem::update>("Update");
        createProcess<&TestSystem::differentThread>("TestChain");
        createProcess<&TestSystem::differentInterval>("TestChain", 1.f);
    }


    void onPlayerMove(player_move_action* action)
	  {
        std::cout << action->value << std::endl;

	      if(action->value < 0)
            std::cout << "Player Move Forward";
	  }


    void update(time::time_span<fast_time> deltaTime)
    {
        static auto query = createQuery<sah>();

        static time::time_span<fast_time> buffer;
        static int frameCount;
        static time::time_span<fast_time> accumulated;

        buffer += deltaTime;
        accumulated += deltaTime;
        frameCount++;

        if (buffer > 1.f)
        {
            buffer -= 1.f;

            for (auto entity : query)
            {
                auto comp = entity.get_component<sah>();
                comp.write({ frameCount });
                std::cout << "component value: " << comp.read().value << std::endl;
            }

            std::cout << "Hi! " << (frameCount / accumulated) << "fps " << deltaTime.milliseconds() << "ms" << std::endl;
        }
    }

    void differentInterval(time::time_span<fast_time> deltaTime)
    {
        static time::time_span<fast_time> buffer;
        static int frameCount;
        static time::time_span<fast_time> accumulated;

        buffer += deltaTime;
        accumulated += deltaTime;
        frameCount++;

        math::vec2 v;
        v.x = 10;
        v.y = 20;

        if (buffer > 1.f)
        {
            buffer -= 1.f;
            std::cout << "This is a fixed interval!! " << (frameCount / accumulated) << "fps " << deltaTime.milliseconds() << "ms" << std::endl;
        }
    }

    void differentThread(time::time_span<fast_time> deltaTime)
    {
        static auto query = createQuery<sah>();

        static time::time_span<fast_time> buffer;
        static int frameCount;
        static time::time_span<fast_time> accumulated;

        buffer += deltaTime;
        accumulated += deltaTime;
        frameCount++;

        if (buffer > 1.f)
        {
            buffer -= 1.f;

            for (auto entity : query)
            {
                auto comp = entity.get_component<sah>();
                std::cout << "component value on different thread: " << comp.read().value << std::endl;
            }

            std::cout << "This is a different thread!! " << (frameCount / accumulated) << "fps " << deltaTime.milliseconds() << "ms" << std::endl;
        }
    }
};
