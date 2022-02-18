#pragma once
#include <physics/diviner/broadphasecollisionalgorithms/broadphasecollisionalgorithm.hpp>

namespace legion::physics
{
    class BroadphaseBruteforce : public BroadPhaseCollisionAlgorithm
    {
    public:

        /**@brief Collects collider pairs that have a chance of colliding and should be checked in narrow-phase collision detection
         * @param manifoldPrecursors all the physics components
         * @param manifoldPrecursorGrouping a list-list of colliders that have a chance of colliding and should be checked
         */
        const std::vector<std::vector<physics_manifold_precursor>>& collectPairs(
            std::vector<physics_manifold_precursor>&& manifoldPrecursors) override
        {
            //log::debug("Brute force!");
            m_groupings.clear();
            m_groupings.emplace_back(std::move(manifoldPrecursors));
            return m_groupings;
        }
    };
}
