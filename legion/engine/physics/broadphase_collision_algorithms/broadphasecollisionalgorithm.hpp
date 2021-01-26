#pragma once
#include <core/core.hpp>
#include <physics/components/physics_component.hpp>
#include <physics/data/physics_manifold_precursor.hpp>
#include <physics/data/physics_manifold.hpp>

namespace legion::physics
{
    class BroadPhaseCollisionAlgorithm
    {
    public:
        BroadPhaseCollisionAlgorithm()
        {

        }

        virtual void collectPairs(std::vector<physics_manifold_precursor>& manifoldPrecursors,
            std::vector<std::vector<physics_manifold_precursor>>& manifoldPrecursorGrouping) = 0;
    };
}
