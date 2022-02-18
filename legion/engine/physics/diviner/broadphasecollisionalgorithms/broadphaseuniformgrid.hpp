#pragma once
#include <physics/diviner/broadphasecollisionalgorithms/broadphasecollisionalgorithm.hpp>
#include <physics/diviner/physics_statics.hpp>

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
        /**@brief Constructor of BroadPhaseUniformGrid
         * @param cellSize The size of the cells for the grid
         * @param emptyCellDestroyThreshold the empty cell count at which the entire uniform grid is reset and rebuild.
         * This can be usefull since every empty cell still takes up space in lists. when a new object is added to the list the list may have to be moved in memory, 
         * which takes time. By keeping the lists small and removing empty cells lists require less moving.
         */
        BroadphaseUniformGrid(math::ivec3 cellSize, size_type emptyCellDestroyThreshold = 0) : m_cellSize(cellSize), m_emptyCellDestroyThreshold(emptyCellDestroyThreshold)
        {
        }

        /**@brief Collects collider pairs that have a chance of colliding and should be checked in narrow-phase collision detection
         * @param manifoldPrecursors all the physics components
         * @return a list-list of colliders that have a chance of colliding and should be checked
         */
        const std::vector<std::vector<physics_manifold_precursor>>& collectPairs(
            std::vector<physics_manifold_precursor>&& manifoldPrecursors) override;

        /**@brief Collects collider pairs that have a chance of colliding and should be checked in narrow-phase collision detection.
         * Unlike collectPairs this function ignores all the cached data and reconstructs the grid every time
         * @param manifoldPrecursors all the physics components
         * @return a list-list of colliders that have a chance of colliding and should be checked
         */
        const std::vector<std::vector<physics_manifold_precursor>>& reConstruct(
            std::vector<physics_manifold_precursor>&& manifoldPrecursors);

        /**@brief Sets the cell size which will be used for the virtual grid
         */
        void setCellSize(math::ivec3 cellSize)
        {
            // Grid size changes, therefore our cached data is no longer valid
            cellIndices.clear();
            m_collectedEntities.clear();
            m_oldPositions.clear();
            m_groupings.clear();
            m_entityOccupiedCells.clear();
            m_emptyCells.clear();

            m_cellSize = cellSize;
        }

        void debugDraw() override;

    private:
        math::ivec3 m_cellSize;
        size_type m_emptyCellDestroyThreshold = 0;

        /**@brief Calculates a cell index from a point. i.e. calculates in which cell in the uniform grid a point will be. 
         */
        math::ivec3 calculateCellIndex(const math::vec3 point);

        // Stores the cell index (ivec3) to the index in the m_groupings list.
        std::unordered_map<math::ivec3, int> cellIndices;

        // All the entites in the cells. Using this new entities can be detected.
        std::unordered_set<id_type> m_collectedEntities;
        // Stores the old positions of the collected entities. This way we can check if an object moved.
        std::unordered_map<id_type, math::vec3> m_oldPositions;
        // Stores all the cells that an entity occupies. An entity can be on the edge of mulitple cells and therefore be in multiple cells at once.
        std::unordered_map<id_type, std::unordered_set<math::ivec3>> m_entityOccupiedCells;

        // All the empty cells. This list is used to check how many empty cells there are.
        std::unordered_set<math::ivec3> m_emptyCells;
    };
}
