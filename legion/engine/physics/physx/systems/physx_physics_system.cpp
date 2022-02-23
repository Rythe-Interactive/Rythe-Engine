#include "physx_physics_system.hpp"
#include <physx/PxPhysicsAPI.h>

namespace legion::physics
{
    using namespace physx;

    PhysXPhysicsSystem::physXStatics PhysXPhysicsSystem::sPhysicsStatics;
    size_type PhysXPhysicsSystem::sSelfInstanceCounter = 0;

    PxPhysics* gPhysics = nullptr;

    //PhysX PVD Debugger Related
    constexpr char* pvdHost = "127.0.0.1";
    constexpr size_type defaultPVDListeningPort = 5425;
    constexpr size_type defaultPVDHostTimeout = 10;

    PhysXPhysicsSystem::PhysXPhysicsSystem()
    {
        sSelfInstanceCounter++;
    }

    void PhysXPhysicsSystem::setup()
    {
        lazyInitPhysXVariables();
        setupDefaultScene();

        createProcess<&PhysXPhysicsSystem::fixedUpdate>("Physics", m_timeStep);
    }

    PhysXPhysicsSystem::~PhysXPhysicsSystem()
    {
        m_physxScene->release();

        sSelfInstanceCounter--;

        if (sSelfInstanceCounter == 0)
        {
            releasePhysXVariables();
        }
    }

    void PhysXPhysicsSystem::lazyInitPhysXVariables()
    {
        static PxDefaultAllocator sDefaultAllocator;
        static PxDefaultErrorCallback sDefaultErrorCallback;

        if (!sPhysicsStatics.m_isInit)
        {
            sPhysicsStatics.foundation = PxCreateFoundation(PX_PHYSICS_VERSION, sDefaultAllocator, sDefaultErrorCallback);

            sPhysicsStatics.pvd = PxCreatePvd(*sPhysicsStatics.foundation);
            PxPvdTransport* transport = PxDefaultPvdSocketTransportCreate(pvdHost, defaultPVDListeningPort, defaultPVDHostTimeout);
            sPhysicsStatics.pvd->connect(*transport, PxPvdInstrumentationFlag::eDEBUG);

            sPhysicsStatics.dispatcher = PxDefaultCpuDispatcherCreate(0); //deal with multithreading later on
        }

        if (!gPhysics)
        {
            gPhysics = PxCreatePhysics(PX_PHYSICS_VERSION, *sPhysicsStatics.foundation, PxTolerancesScale(), true, sPhysicsStatics.pvd);
        }

        m_defaultAllocator = &sDefaultAllocator;
        m_defaultErrorCallback = &sDefaultErrorCallback;
    }

    void PhysXPhysicsSystem::setupDefaultScene()
    {
        PxSceneDesc sceneDesc(gPhysics->getTolerancesScale());
        sceneDesc.gravity = PxVec3(0.0f, -9.81f, 0.0f);
        sceneDesc.cpuDispatcher = sPhysicsStatics.dispatcher;
        sceneDesc.filterShader = PxDefaultSimulationFilterShader;
        sceneDesc.flags |= PxSceneFlag::eENABLE_STABILIZATION;

        m_physxScene = gPhysics->createScene(sceneDesc);
    }

    void PhysXPhysicsSystem::releasePhysXVariables()
    {
        sPhysicsStatics.dispatcher->release();
        gPhysics->release();

        if (sPhysicsStatics.pvd)
        {
            PxPvdTransport* transport = sPhysicsStatics.pvd->getTransport();
            sPhysicsStatics.pvd->release();
            sPhysicsStatics.pvd = nullptr;
            transport->release();
        }

        sPhysicsStatics.foundation->release();
    }

    void PhysXPhysicsSystem::fixedUpdate(time::time_span<fast_time> deltaTime)
    {
        m_physxScene->simulate(m_timeStep);
        m_physxScene->fetchResults(true);
    }
}
