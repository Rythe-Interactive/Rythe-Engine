#include <physics/systems/physicssystem.hpp>


namespace legion::physics
{
    std::vector<std::shared_ptr<physics::PenetrationQuery>> PhysicsSystem::penetrationQueries;
    std::vector<math::vec3> PhysicsSystem::aPoint;
    std::vector<math::vec3> PhysicsSystem::bPoint;
    std::vector<physics_contact> PhysicsSystem::contactPoints;
    bool PhysicsSystem::IsPaused = true;
    bool PhysicsSystem::oneTimeRunActive = false;


    void PhysicsSystem::bruteForceBroadPhase(std::vector<physics_manifold_precursor>& manifoldPrecursors,
        std::vector<std::vector<physics_manifold_precursor>>& manifoldPrecursorGrouping)
    {
        manifoldPrecursorGrouping.push_back(std::move(manifoldPrecursors));
    }
}
