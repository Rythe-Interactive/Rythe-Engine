#pragma once

#include <rendering/data/Octree.hpp>
namespace legion::rendering
{
    /**@struct point emitter
     * @brief contains data for LODing the point cloud, stores currentLOD, the octree for the LOD, and ElementsPerLOD which define how many particles an LOD stores
     */
    struct point_emitter_data
    {
        int CurrentLOD = 0;
        rendering::Octree<uint8>* Tree;
        std::vector<int> ElementsPerLOD;
    };
}
