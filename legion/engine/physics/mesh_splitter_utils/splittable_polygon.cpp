#include "splittable_polygon.h"

namespace legion::physics
{
    SplittablePolygon::SplittablePolygon
    (std::vector<std::shared_ptr<MeshHalfEdge>> pEdgesInMesh)
        : edgesInMesh(std::move(pEdgesInMesh))
    {
        debugColor = math::color(math::linearRand(0.25f, 0.7f), math::linearRand(0.25f, 0.7f), math::linearRand(0.25f, 0.7f));
    }

    std::vector<std::shared_ptr<MeshHalfEdge>>& SplittablePolygon::GetMeshEdges()
    {
        return edgesInMesh;
    }

    void SplittablePolygon::calculateLocalCentroid()
    {
        for (auto edge : edgesInMesh)
        {
            localCentroid += edge->position;
        }

        localCentroid /= (float)edgesInMesh.size();
    }

}

