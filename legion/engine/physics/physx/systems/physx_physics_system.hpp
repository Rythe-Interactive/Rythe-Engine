#pragma once
#include <core/core.hpp>

namespace physx
{
    class PxScene;
};

namespace legion::physics
{
    class PhysXPhysicsSystem final : public System<PhysXPhysicsSystem>
    {
    public:

        virtual void setup();

        void fixedUpdate(time::time_span<fast_time> deltaTime);

    private:

        void setupPhysXGlobalVariables();

        void setupDefaultScene();

        float m_timeStep = 0.02f;

        physx::PxScene* m_physxScene;
    };
};
