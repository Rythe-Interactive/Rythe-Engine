#include <physics/broadphasecollisionalgorithms/broadphaseuniformgrid.hpp>
#include <physics/physics_contact.hpp>


namespace legion::physics
{
    const std::vector<std::vector<physics_manifold_precursor>>& BroadphaseUniformGrid::collectPairs(
        std::vector<physics_manifold_precursor>&& manifoldPrecursors)
    {
        OPTICK_EVENT();

        // Check if the amount of empty cells is higher than the threshhold. If it is, clear the cached data
        if (m_emptyCellDestroyThreshold > 0 && m_emptyCells.size() > m_emptyCellDestroyThreshold)
        {
            // This function is called since it clears all data. A few cpu cycles are lost to settings the m_cellsize to itself.
            setCellSize(m_cellSize);
        }

        for (auto& precursor : manifoldPrecursors)
        {
            OPTICK_EVENT("Processing entity");

            math::vec3 pos;
            pos.x = precursor.worldTransform[3].x;
            pos.y = precursor.worldTransform[3].y;
            pos.z = precursor.worldTransform[3].z;
            id_type id = precursor.entity;
            if (m_collectedEntities.count(id))
            {
                // If the entities position has not changed we can skip it.
                if (pos == m_oldPositions.at(id))
                {
                    continue;
                }
                else
                {
                    // The entity moved, so we store its new position
                    m_oldPositions.at(id) = pos;
                }
            }
            else
            {
                // A new entity has entered the broadphase, prepare its data
                m_collectedEntities.insert(id);
                m_oldPositions.emplace(id, pos);
                m_entityOccupiedCells.emplace(id, std::unordered_set<math::ivec3>());
            }

            // Get all the colliders of the entity
            std::vector<legion::physics::PhysicsColliderPtr>& colliders = precursor.physicsComp->colliders;
            // If the entity has no colliders, we can skip it
            if (colliders.size() == 0) continue;

            // Get the biggest AABB collider of this physics component
            // If it has one collider we can simply retrieve it
            // Oherwise we have to combine the bounds of all its colliders
            std::pair<math::vec3, math::vec3> aabb = colliders.at(0)->GetMinMaxWorldAABB();
            for (int i = 1; i < colliders.size(); ++i)
            {
                aabb = PhysicsStatics::CombineAABB(colliders.at(i)->GetMinMaxWorldAABB(), aabb);
            }

            // We get the start and end cell indices for this entity. This way we know which groupings/cells should contain this entity
            math::ivec3 startCellIndex = calculateCellIndex(aabb.first);
            math::ivec3 endCellIndex = calculateCellIndex(aabb.second);

            {
                OPTICK_EVENT("Iterating overlapping cells");

                // We store visitedCellIndex since the entities old occupied cells may change and may have to be removed
                std::unordered_set<math::ivec3> visitedCells;

                for (int x = startCellIndex.x; x <= endCellIndex.x; ++x)
                {
                    for (int y = startCellIndex.y; y <= endCellIndex.y; ++y)
                    {
                        for (int z = startCellIndex.z; z <= endCellIndex.z; ++z)
                        {
                            OPTICK_EVENT("Inserting entity");
                            // Get current cell index
                            math::ivec3 currentCellIndex = math::ivec3(x, y, z);
                            // Store cell in visited
                            visitedCells.insert(currentCellIndex);

                            // If the entity already occupies this cell, skip it
                            if (m_entityOccupiedCells.at(id).count(currentCellIndex)) continue;

                            // Add this cell to the cells of the entity
                            m_entityOccupiedCells.at(id).insert(currentCellIndex);

                            // If the current cell already exist we can push this object into that cell
                            // otherwise the cell is created
                            if (cellIndices.count(currentCellIndex))
                            {
                                // Cell already exist, push this object into it
                                m_groupings.at(cellIndices.at(currentCellIndex)).push_back(precursor);

                                // We do not need to check if the object was already in the set, since the set will do nothing if it wasn't there to be removed
                                m_emptyCells.erase(currentCellIndex);
                            }
                            else
                            {
                                // A new cell is created
                                cellIndices.emplace(currentCellIndex, m_groupings.size());
                                m_groupings.emplace_back();
                                m_groupings.at(m_groupings.size() - 1).push_back(precursor);

                                // Since the cell is new, it will never have existed in the empty cells list
                            }
                        }
                    }
                }

                // We keep track of the cells that are to be removed from the occupied cells list of the entity
                std::vector<math::ivec3> toRemove;
                for (auto& cellIndex : m_entityOccupiedCells.at(id))
                {
                    // If the occupied cell has not been visisted this iteration
                    // Remove if from groupings
                    if (!visitedCells.count(cellIndex))
                    {
                        int index = cellIndices.at(cellIndex);
                        m_groupings.at(index).erase(
                            std::remove(
                                m_groupings.at(cellIndices.at(cellIndex)).begin(),
                                m_groupings.at(cellIndices.at(cellIndex)).end(),
                                precursor), m_groupings.at(cellIndices.at(cellIndex)).end()
                        );
                        // If the cell is now empty, store it in the empty cell list
                        if (m_groupings.at(index).size() == 0)
                        {
                            m_emptyCells.insert(cellIndex);
                        }
                        // Push this cell into the toRemove
                        toRemove.push_back(cellIndex);
                    }
                }
                // Remove all the toRemove cells from the entity occupied cells
                for (auto& item : toRemove)
                {
                    m_entityOccupiedCells.at(id).erase(item);
                }

            }
        }

        return m_groupings;
    }

    const std::vector<std::vector<physics_manifold_precursor>>& BroadphaseUniformGrid::reConstruct(
        std::vector<physics_manifold_precursor>&& manifoldPrecursors)
    {
        std::vector<std::vector<physics_manifold_precursor>> groupings;
        OPTICK_EVENT();
        std::unordered_map<math::ivec3, int> cellIndices;
        for (auto& precursor : manifoldPrecursors)
        {
            OPTICK_EVENT("Processing entity");

            std::vector<legion::physics::PhysicsColliderPtr>& colliders = precursor.physicsComp->colliders;
            if (colliders.size() == 0) continue;

            // Get the biggest AABB collider of this physics component
            // If it has one collider we can simply retrieve it
            // Oherwise we have to combine the bounds of all its colliders
            std::pair<math::vec3, math::vec3> aabb = colliders.at(0)->GetMinMaxWorldAABB();
            for (int i = 1; i < colliders.size(); ++i)
            {
                aabb = PhysicsStatics::CombineAABB(colliders.at(i)->GetMinMaxWorldAABB(), aabb);
            }
            math::ivec3 startCellIndex = calculateCellIndex(aabb.first);
            math::ivec3 endCellIndex = calculateCellIndex(aabb.second);

            {
                OPTICK_EVENT("Iterating overlapping cells");

                for (int x = startCellIndex.x; x <= endCellIndex.x; ++x)
                {
                    for (int y = startCellIndex.y; y <= endCellIndex.y; ++y)
                    {
                        for (int z = startCellIndex.z; z <= endCellIndex.z; ++z)
                        {
                            OPTICK_EVENT("Inserting entity");
                            math::ivec3 currentCellIndex = math::ivec3(x, y, z);
                            if (cellIndices.count(currentCellIndex))
                            {
                                groupings.at(cellIndices.at(currentCellIndex)).push_back(precursor);
                            }
                            else
                            {
                                cellIndices.emplace(currentCellIndex, groupings.size());
                                groupings.emplace_back();
                                groupings.at(groupings.size() - 1).push_back(precursor);
                            }
                        }
                    }
                }
            }
        }
        return groupings;
    }

    math::ivec3 BroadphaseUniformGrid::calculateCellIndex(const math::vec3 point)
    {
        // A point below 0 needs an extra 'push' since -0.5 will be cast to int as 0
        math::vec3 temp = point;
        if (temp.x < 0) temp.x-=m_cellSize.x;
        if (temp.y < 0) temp.y-=m_cellSize.y;
        if (temp.z < 0) temp.z-=m_cellSize.z;

        math::ivec3 cellIndex = math::ivec3(temp.x / (float)m_cellSize.x, temp.y / (float)m_cellSize.y, temp.z / (float)m_cellSize.z);

        return cellIndex;
    }

    void BroadphaseUniformGrid::debugDraw()
    {
        for (auto& [index, listIndex] : cellIndices)
        {
            int childCount = m_groupings.at(listIndex).size();
            math::ivec3 pos = index * m_cellSize;
            debug::drawCube(pos, pos + m_cellSize, childCount > 1 ? math::colors::red : math::colors::blue, 5.0f);
        }
    }
}
