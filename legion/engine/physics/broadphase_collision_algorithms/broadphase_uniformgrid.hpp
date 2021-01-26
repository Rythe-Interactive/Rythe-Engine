#pragma once
#include <physics/broadphase_collision_algorithms/broadphasecollisionalgorithm.hpp>
#include <physics/physics_statics.hpp>

namespace legion::physics
{

    class BroadphaseUniformGrid : public BroadPhaseCollisionAlgorithm
    {
    public:
        BroadphaseUniformGrid(math::ivec3 cellSize) : m_cellSize(cellSize)
        {
        }

        void collectPairs(std::vector<physics_manifold_precursor>& manifoldPrecursors,
            std::vector<std::vector<physics_manifold_precursor>>& manifoldPrecursorGrouping) override;

        void setCellSize(math::ivec3 cellSize)
        {
            m_cellSize = cellSize;
        }

    private:
        math::ivec3 m_cellSize;

        math::ivec3 calculateCellIndex(const math::vec3 point);
    };
}
