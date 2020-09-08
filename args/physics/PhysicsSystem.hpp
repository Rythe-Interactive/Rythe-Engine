#pragma once
#ifndef ARGS_IMPORT
#define ARGS_IMPORT
#include <core/core.hpp>
#include <core/platform/args_library.hpp>
#else
#include <core/core.hpp>
#endif // !ARGS_IMPORT

#include <core/math/math.hpp>

//#include <application/application.hpp>

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
            
            tickTimeRemaining += deltaTime;
            int currentTickCount = 0;

            while (shouldPhysicsStillTick(currentTickCount))
            {
                float tick{};
                decreaseTimeRemaining(tickTimeRemaining,tick);

                runPhysicsPipeline();

                integrateRigidbodies(tick);

                currentTickCount++;
            }

            integrateRigidbodies(math::clamp(tickTimeRemaining,0.0f, timeStep));

        }

    private:

        float tickTimeRemaining;
        const float maxTickCount = 3;
        const float timeStep = 0.02f;

        //TODO Physics Documentation
        /*
        
        */
        bool shouldPhysicsStillTick(int currentTick)
        {
            return tickTimeRemaining > timeStep && currentTick <= maxTickCount;
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
            if (tickTimeRemaining > timeStep)
            {
                tickTimeRemaining -= timeStep;
                tick = timeStep;
                return;
            }

            tick = 0.0f;

        }





    };
}

