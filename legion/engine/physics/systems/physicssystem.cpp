#include <physics/systems/physicssystem.hpp>


namespace legion::physics
{
    std::unique_ptr<BroadPhaseCollisionAlgorithm> PhysicsSystem::m_broadPhase = nullptr;

    bool PhysicsSystem::IsPaused = false;
    bool PhysicsSystem::oneTimeRunActive = false;


    void PhysicsSystem::setup()
    {
        createProcess<&PhysicsSystem::fixedUpdate>("Physics", m_timeStep);

        manifoldPrecursorQuery = createQuery<position, rotation, scale, physicsComponent>();
        //BroadphaseBruteforce
        //BroadphaseUniformGrid

        //std::make_unique<BroadphaseUniformGrid>(math::vec3(2,2,2));
        //std::make_unique<BroadphaseBruteforce>();

        m_broadPhase = std::make_unique<BroadphaseBruteforce>();

    }
}
