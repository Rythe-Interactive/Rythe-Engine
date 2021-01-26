#pragma once
#include <physics/broadphasecollisionalgorithms/broadphasecollisionalgorithm.hpp>
#include <physics/physics_statics.hpp>

namespace legion::physics
{
    /**@class BroadphaseUniformGrid
     * @brief Implementation of broad-phase collision detection
     * Creates a virtual axis-aligned grid in which it sorts all physics components with colliders.
     * All colliders in 1 cell have a possibility of colliding, and will be checked in narrow-phase 
     */
    class BroadphaseUniformGrid : public BroadPhaseCollisionAlgorithm
    {
    public:
        BroadphaseUniformGrid(math::ivec3 cellSize) : m_cellSize(cellSize)
        {
        }

        /**@brief Collects collider pairs that have a chance of colliding and should be checked in narrow-phase collision detection
         * @param manifoldPrecursors all the physics components
         * @param manifoldPrecursorGrouping a list-list of colliders that have a chance of colliding and should be checked
         */
        void collectPairs(std::vector<physics_manifold_precursor>& manifoldPrecursors,
            std::vector<std::vector<physics_manifold_precursor>>& manifoldPrecursorGrouping) override;

        /**@brief Sets the cell size which will be used for the virtual grid
         */
        void setCellSize(math::ivec3 cellSize)
        {
            m_cellSize = cellSize;
        }

    private:
        math::ivec3 m_cellSize;

        /**@brief Calculates a cell index from a point. i.e. calculates in which cell in the uniform grid a point will be. 
         */
        math::ivec3 calculateCellIndex(const math::vec3 point);
    };
}
