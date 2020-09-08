#pragma once

#ifndef ARGS_IMPORT
#define ARGS_IMPORT
#include <core/core.hpp>
#include <core/platform/args_library.hpp>
#else
#include <core/core.hpp>
#endif // !ARGS_IMPORT

#include <core/math/math.hpp>

namespace args::physics
{
    class PhysicsSystem final : public System<PhysicsSystem>
    {
    public:

        virtual void setup()
        {
            createProcess<&PhysicsSystem::fixedUpdate>("Physics", 1 / 60.0f);
        }

        void fixedUpdate(time::time_span<fast_time> deltaTime)
        {
            //std::cout << "physics fixed update " << deltaTime << std::endl;
            
            m_tickTimeRemaining += deltaTime;
            //std::cout << "tickTimeRemaining " << tickTimeRemaining << std::endl;
            int currentTickCount = 0;

            while (shouldPhysicsStillTick(currentTickCount))
            {
                float tick{};
                decreaseTimeRemaining(m_tickTimeRemaining,tick);

                runPhysicsPipeline();

                integrateRigidbodies(tick);

                currentTickCount++;
            }

            //std::cout << "leftover integrate " << tickTimeRemaining << std::endl;
            integrateRigidbodies(math::clamp(m_tickTimeRemaining,0.0f, m_timeStep));

        }

    private:

        float m_tickTimeRemaining;
        const float m_maxTickCount = 3;
        const float m_timeStep = 0.02f;
        
        bool shouldPhysicsStillTick(int currentTick)
        {
            return m_tickTimeRemaining > m_timeStep && currentTick <= m_maxTickCount;
        }

        void runPhysicsPipeline()
        {
            //Broadphase Optimization

            //Narrophase 

            //Collision Resolution

        }

        void integrateRigidbodies(float deltaTime)
        {

        }

        void decreaseTimeRemaining(float& tickTimeRemaining,float & tick)
        {
            if (tickTimeRemaining > m_timeStep)
            {
                tickTimeRemaining -= m_timeStep;
                tick = m_timeStep;
                return;
            }

            tick = 0.0f;

        }
    };
}

