#pragma once
#include <physics/broadphase_collision_algorithms/broadphasecollisionalgorithm.hpp>

namespace legion::physics
{
    class BroadphaseBruteforce : public BroadPhaseCollisionAlgorithm
    {
    public:

        void collectPairs(std::vector<physics_manifold_precursor>& manifoldPrecursors,
            std::vector<std::vector<physics_manifold_precursor>>& manifoldPrecursorGrouping) override
        {
            manifoldPrecursorGrouping.push_back(std::move(manifoldPrecursors));
        }
    };
}
