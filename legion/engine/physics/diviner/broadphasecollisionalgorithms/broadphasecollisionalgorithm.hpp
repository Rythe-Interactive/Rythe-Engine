#pragma once
#include <core/core.hpp>
#include <physics/diviner/components/physics_component.hpp>
#include <physics/diviner/data/physics_manifold_precursor.hpp>
#include <physics/diviner/data/physics_manifold.hpp>

namespace legion::physics
{
    /**@class BroadPhaseCollisionAlgorithm
     * @brief The base class for a broad phase collision detection
     */
    class BroadPhaseCollisionAlgorithm
    {
    public:
        /**@brief Collects collider pairs that have a chance of colliding and should be checked in narrow-phase collision detection
         * @param manifoldPrecursors all the physics components 
         * @param manifoldPrecursorGrouping a list-list of colliders that have a chance of colliding and should be checked
         */
        virtual const std::vector<std::vector<physics_manifold_precursor>>& collectPairs(
            std::vector<physics_manifold_precursor>&& manifoldPrecursors) LEGION_PURE;

        virtual void debugDraw()
        {

        }

    protected:
        std::vector<std::vector<physics_manifold_precursor>> m_groupings;
    };
}
