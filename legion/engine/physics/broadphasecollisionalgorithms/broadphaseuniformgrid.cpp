#include <physics/broadphasecollisionalgorithms/broadphaseuniformgrid.hpp>
#include <physics/physics_contact.hpp>

namespace legion::physics
{
    void BroadphaseUniformGrid::collectPairs(std::vector<physics_manifold_precursor>& manifoldPrecursors,
        std::vector<std::vector<physics_manifold_precursor>>& manifoldPrecursorGrouping)
    {
        std::unordered_map<math::ivec3, int> cellIndices;
        for (auto& precursor : manifoldPrecursors)
        {
            std::vector<legion::physics::PhysicsColliderPtr> colliders = precursor.physicsComponentHandle.read().colliders;
            if (colliders.size() == 0) continue;

            // Get the biggest AABB collider of this physics component
            // If it has one collider we can simply retrieve it
            // Oherwise we have to combine the bounds of all its colliders
            std::tuple<math::vec3, math::vec3> aabb = colliders.at(0)->GetMinMaxWorldAABB();
            for (int i = 1; i < colliders.size(); ++i)
            {
                aabb = PhysicsStatics::CombineAABB(colliders.at(i)->GetMinMaxWorldAABB(), aabb);
            }
            math::ivec3 startCellIndex = calculateCellIndex(std::get<0>(aabb));
            math::ivec3 endCellIndex = calculateCellIndex(std::get<1>(aabb));
            for (int x = startCellIndex.x; x <= endCellIndex.x; ++x)
            {
                for (int y = startCellIndex.y; y <= endCellIndex.y; ++y)
                {
                    for (int z = startCellIndex.z; z <= endCellIndex.z; ++z)
                    {
                        math::ivec3 currentCellIndex = math::ivec3(x, y, z);
                        if (cellIndices.find(currentCellIndex) != cellIndices.end())
                        {
                            manifoldPrecursorGrouping.at(cellIndices.at(currentCellIndex)).push_back(precursor);
                        }
                        else
                        {
                            cellIndices.emplace(currentCellIndex, manifoldPrecursorGrouping.size());
                            manifoldPrecursorGrouping.push_back(std::vector<physics_manifold_precursor>());
                            manifoldPrecursorGrouping.at(manifoldPrecursorGrouping.size() - 1).push_back(precursor);
                        }
                    }
                }
            }
        }

        /*for (auto& [index, precursorIndex] : cellIndices )
        {
            int childCount = manifoldPrecursorGrouping.at(precursorIndex).size();

            debug::drawCube(index, index + m_cellSize, childCount > 1 ? math::colors::red:math::colors::blue, 5.0f);
        }*/
    }

    math::ivec3 BroadphaseUniformGrid::calculateCellIndex(const math::vec3 point)
    {
        // A point below 0 needs an extra 'push' since -0.5 will be cast to int as 0
        math::vec3 temp = point;
        if (temp.x < 0) --temp.x;
        if (temp.y < 0) --temp.y;
        if (temp.z < 0) --temp.z;

        math::ivec3 cellIndex = math::ivec3(temp.x / (float)m_cellSize.x, temp.y / (float)m_cellSize.y, temp.z / (float)m_cellSize.z);

        return cellIndex;
    }
}
