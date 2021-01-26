#pragma once
#include <physics/broadphasecollisionalgorithms/broadphasecollisionalgorithm.hpp>

namespace legion::physics
{
    class BroadphaseBruteforce : public BroadPhaseCollisionAlgorithm
    {
    public:

        /**@brief Collects collider pairs that have a chance of colliding and should be checked in narrow-phase collision detection
         * @param manifoldPrecursors all the physics components
         * @param manifoldPrecursorGrouping a list-list of colliders that have a chance of colliding and should be checked
         */
        void collectPairs(std::vector<physics_manifold_precursor>&& manifoldPrecursors,
            std::vector<std::vector<physics_manifold_precursor>>& manifoldPrecursorGrouping) override
        {
            log::debug("Brute force!");
            manifoldPrecursorGrouping.emplace_back(std::move(manifoldPrecursors));
        }
    };
}
