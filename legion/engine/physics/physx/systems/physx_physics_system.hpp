#pragma once
#include <core/core.hpp>

namespace physx
{
    class PxScene;
    class PxFoundation;
    class PxPvd;
    class PxDefaultCpuDispatcher;
    class PxDefaultAllocator;
    class PxDefaultErrorCallback;
};

namespace legion::physics
{
    class PhysXPhysicsSystem final : public System<PhysXPhysicsSystem>
    {
    public:

        PhysXPhysicsSystem();
        ~PhysXPhysicsSystem();

        virtual void setup();

        void fixedUpdate(time::time_span<fast_time> deltaTime);

    private:

        struct physXStatics
        {
            bool m_isInit = false;

            physx::PxFoundation* foundation = nullptr;
            physx::PxPvd* pvd = nullptr;
            physx::PxDefaultCpuDispatcher* dispatcher = nullptr;
        };

        void lazyInitPhysXVariables();

        void releasePhysXVariables();

        void setupDefaultScene();

        static constexpr float m_timeStep = 0.02f;

        physx::PxScene* m_physxScene;

        physx::PxDefaultAllocator* m_defaultAllocator = nullptr;
        physx::PxDefaultErrorCallback* m_defaultErrorCallback = nullptr;

        static physXStatics sPhysicsStatics;

        static size_type sSelfInstanceCounter;
    };
};
