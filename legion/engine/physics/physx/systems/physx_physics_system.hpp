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

        virtual void shutdown();

        void fixedUpdate(time::time_span<fast_time> deltaTime);

    private:

        struct PhysxStatics;

        void lazyInitPhysXVariables();

        void releasePhysXVariables();

        void setupDefaultScene();

        static constexpr float m_timeStep = 0.02f;

        physx::PxScene* m_physxScene;

        std::mutex m_setupShutdownMutex;
    };
};
