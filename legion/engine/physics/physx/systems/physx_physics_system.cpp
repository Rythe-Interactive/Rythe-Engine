#include "physx_physics_system.hpp"
#include <physx/PxPhysicsAPI.h>

namespace legion::physics
{
    using namespace physx;

    PxDefaultAllocator		gAllocator;
    PxDefaultErrorCallback	gErrorCallback;

    PxFoundation* gFoundation = nullptr;
    PxPvd* gPvd = NULL;

    PxPhysics* gPhysics = NULL;

    PxDefaultCpuDispatcher* gDispatcher = NULL;

    //PhysX PVD Debugger Related
    constexpr char* pvdHost = "127.0.0.1";
    constexpr size_type defaultPVDListeningPort = 5425;
    constexpr size_type defaultPVDHostTimeout = 10;

    void PhysXPhysicsSystem::setup()
    {
        setupPhysXGlobalVariables();
        setupDefaultScene();
        
        createProcess<&PhysXPhysicsSystem::fixedUpdate>("Physics", m_timeStep);
    }

    void PhysXPhysicsSystem::fixedUpdate(time::time_span<fast_time> deltaTime)
    {
        m_physxScene->simulate(m_timeStep);
        m_physxScene->fetchResults(true);
    }

    void PhysXPhysicsSystem::setupPhysXGlobalVariables()
    {
        gFoundation = PxCreateFoundation(PX_PHYSICS_VERSION, gAllocator, gErrorCallback);

        PxPvd* pvd = PxCreatePvd(*gFoundation);
        PxPvdTransport* transport = PxDefaultPvdSocketTransportCreate(pvdHost, defaultPVDListeningPort, defaultPVDHostTimeout);
        pvd->connect(*transport, PxPvdInstrumentationFlag::eDEBUG);

        gPhysics = PxCreatePhysics(PX_PHYSICS_VERSION, *gFoundation, PxTolerancesScale(), true, pvd);
    }

    void PhysXPhysicsSystem::setupDefaultScene()
    {
        gDispatcher = PxDefaultCpuDispatcherCreate(0); //deal with multithreading later on

        PxSceneDesc sceneDesc(gPhysics->getTolerancesScale());
        sceneDesc.gravity = PxVec3(0.0f, -9.81f, 0.0f);
        sceneDesc.cpuDispatcher = gDispatcher;
        sceneDesc.filterShader = PxDefaultSimulationFilterShader;
        sceneDesc.flags |= PxSceneFlag::eENABLE_STABILIZATION;

        m_physxScene = gPhysics->createScene(sceneDesc);
    }
}
