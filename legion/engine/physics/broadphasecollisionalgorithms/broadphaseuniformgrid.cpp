#include <physics/broadphasecollisionalgorithms/broadphaseuniformgrid.hpp>
#include <physics/physics_contact.hpp>

namespace legion::physics
{
    const std::vector<std::vector<physics_manifold_precursor>>& BroadphaseUniformGrid::collectPairs(
        std::vector<physics_manifold_precursor>&& manifoldPrecursors)
    {
        // TO DO
        // NOT CLEAR DATA BY CALLING SETCELLSIZE
        // AND MAKE SURE THE PHYSICS DOES NOT BREAK
        //setCellSize(m_cellSize);
        log::debug("Uniform grid!");
        log::debug("cell size {}", m_cellSize);
        OPTICK_EVENT();
        for (auto& precursor : manifoldPrecursors)
        {
            OPTICK_EVENT("Processing entity");

            math::vec3 pos = precursor.worldTransform[3].xyz;
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
                    m_oldPositions.at(id) = pos;
                }
            }
            else
            {
                // A new entity has entered the broadphase
                m_collectedEntities.insert(id);
                m_oldPositions.emplace(id, pos);
                m_entityOccupiedCells.emplace(id, std::unordered_set<math::ivec3>());
            }

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
                math::ivec3 iterationIndex;

                std::unordered_set<math::ivec3> visitedCells;

                for (int x = startCellIndex.x; x <= endCellIndex.x; ++x)
                {
                    for (int y = startCellIndex.y; y <= endCellIndex.y; ++y)
                    {
                        for (int z = startCellIndex.z; z <= endCellIndex.z; ++z)
                        {
                            OPTICK_EVENT("Inserting entity");
                            math::ivec3 currentCellIndex = math::ivec3(x, y, z);
                            visitedCells.insert(currentCellIndex);

                            if (m_entityOccupiedCells.at(id).count(currentCellIndex)) continue;
                            // Add this cell to the cells of the entity
                            m_entityOccupiedCells.at(id).insert(currentCellIndex);

                            // If the current cell already exist we can push this object into that cell
                            // otherwise the create the cell
                            if (cellIndices.count(currentCellIndex))
                            {
                                m_groupings.at(cellIndices.at(currentCellIndex)).push_back(precursor);
                            }
                            else
                            {
                                cellIndices.emplace(currentCellIndex, m_groupings.size());

                                m_groupings.emplace_back();
                                m_groupings.at(m_groupings.size() - 1).push_back(precursor);
                            }
                            ++iterationIndex.z;
                        }
                        ++iterationIndex.y;
                    }
                    ++iterationIndex.x;
                }

                std::vector<math::ivec3> toRemove;
                for (auto& cellIndex : m_entityOccupiedCells.at(id))
                {
                    if (!visitedCells.count(cellIndex))
                    {
                        m_groupings.at(cellIndices.at(cellIndex)).erase(
                            std::remove(
                                m_groupings.at(cellIndices.at(cellIndex)).begin(),
                                m_groupings.at(cellIndices.at(cellIndex)).end(),
                                precursor), m_groupings.at(cellIndices.at(cellIndex)).end()
                        );
                        toRemove.push_back(cellIndex);
                    }
                }
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
        log::debug("Uniform grid!");
        log::debug("cell size {}", m_cellSize);
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
