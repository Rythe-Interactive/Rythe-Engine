#include <physics/systems/physicssystem.hpp>


namespace legion::physics
{
    std::unique_ptr<BroadPhaseCollisionAlgorithm> PhysicsSystem::m_broadPhase = nullptr;



    bool PhysicsSystem::IsPaused = true;
    bool PhysicsSystem::oneTimeRunActive = false;
}
