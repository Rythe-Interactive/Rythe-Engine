#include "physx_physics_system.hpp"
#include <physx/PxPhysicsAPI.h>

namespace legion::physics
{
    using namespace physx;

    struct physXStatics
    {
        static size_type sSelfInstanceCounter;

        static PxFoundation* foundation;
        static PxPvd* pvd;
        static PxDefaultCpuDispatcher* dispatcher;
        static PxPhysics* phyxSDK;

        static PxDefaultAllocator defaultAllocator;
        static PxDefaultErrorCallback defaultErrorCallback;
    };

    size_type physXStatics::sSelfInstanceCounter = 0;

    PxFoundation* physXStatics::foundation = nullptr;
    PxPvd* physXStatics::pvd = nullptr;
    PxDefaultCpuDispatcher* physXStatics::dispatcher = nullptr;
    PxPhysics* physXStatics::phyxSDK = nullptr;

    PxDefaultAllocator physXStatics::defaultAllocator;
    PxDefaultErrorCallback physXStatics::defaultErrorCallback;
    
    //PhysX PVD Debugger Related
    constexpr const char* pvdHost = "127.0.0.1";
    constexpr size_type defaultPVDListeningPort = 5425;
    constexpr size_type defaultPVDHostTimeout = 10;

    void PhysXPhysicsSystem::setup()
    {
        physXStatics::sSelfInstanceCounter++;

        lazyInitPhysXVariables();
        setupDefaultScene();

        createProcess<&PhysXPhysicsSystem::fixedUpdate>("Physics", m_timeStep);
    }

    void PhysXPhysicsSystem::shutdown()
    {
        m_physxScene->release();

        physXStatics::sSelfInstanceCounter--;

        if (physXStatics::sSelfInstanceCounter == 0)
        {
            releasePhysXVariables();
        }
    }

    void PhysXPhysicsSystem::lazyInitPhysXVariables()
    {
        if (physXStatics::sSelfInstanceCounter == 1)
        {
            physXStatics::foundation = PxCreateFoundation(PX_PHYSICS_VERSION,
                physXStatics::defaultAllocator, physXStatics::defaultErrorCallback);

            physXStatics::pvd = PxCreatePvd(*physXStatics::foundation);
            PxPvdTransport* transport = PxDefaultPvdSocketTransportCreate(pvdHost, defaultPVDListeningPort, defaultPVDHostTimeout);
            physXStatics::pvd->connect(*transport, PxPvdInstrumentationFlag::eDEBUG);

            physXStatics::dispatcher = PxDefaultCpuDispatcherCreate(0); //deal with multithreading later on

            physXStatics::phyxSDK = PxCreatePhysics(PX_PHYSICS_VERSION, *physXStatics::foundation, PxTolerancesScale(), true, physXStatics::pvd);
        }
    }

    void PhysXPhysicsSystem::setupDefaultScene()
    {
        PxSceneDesc sceneDesc(physXStatics::phyxSDK->getTolerancesScale());
        sceneDesc.gravity = PxVec3(0.0f, -9.81f, 0.0f);
        sceneDesc.cpuDispatcher = physXStatics::dispatcher;
        sceneDesc.filterShader = PxDefaultSimulationFilterShader;
        sceneDesc.flags |= PxSceneFlag::eENABLE_STABILIZATION;

        m_physxScene = physXStatics::phyxSDK->createScene(sceneDesc);
    }

    void PhysXPhysicsSystem::releasePhysXVariables()
    {
        physXStatics::dispatcher->release();
        physXStatics::phyxSDK->release();

        if (physXStatics::pvd)
        {
            PxPvdTransport* transport = physXStatics::pvd->getTransport();
            physXStatics::pvd->release();
            physXStatics::pvd = nullptr;
            transport->release();
        }

        physXStatics::foundation->release();
    }

    void PhysXPhysicsSystem::fixedUpdate(time::time_span<fast_time> deltaTime)
    {
        m_physxScene->simulate(m_timeStep);
        m_physxScene->fetchResults(true);
    }
}
