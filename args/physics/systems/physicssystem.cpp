#include <physics/systems/physicssystem.hpp>


namespace args::physics
{
    std::vector<std::shared_ptr<physics::PenetrationQuery>> PhysicsSystem::penetrationQueries;
    std::vector<math::vec3> PhysicsSystem::aPoint;
    std::vector<math::vec3> PhysicsSystem::bPoint;
}
