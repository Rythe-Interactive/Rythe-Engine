#include <physics/mesh_splitter_utils/splittable_polygon.h>
#include <physics/physics_statics.hpp>
#include <physics/physicsconstants.hpp>
#include <physics/mesh_splitter_utils/mesh_half_edge.hpp>

namespace legion::physics
{
    SplittablePolygon::SplittablePolygon
    (std::vector<std::shared_ptr<MeshHalfEdge>> pEdgesInMesh,math::vec3 pNormal)
        : edgesInPolygon(std::move(pEdgesInMesh)), localNormal(pNormal)
    {
        debugColor = math::color(math::linearRand(0.25f, 0.7f), math::linearRand(0.25f, 0.7f), math::linearRand(0.25f, 0.7f));

        CalculateLocalCentroid();
    }

    std::vector<std::shared_ptr<MeshHalfEdge>>& SplittablePolygon::GetMeshEdges()
    {
        return edgesInPolygon;
    }

    void SplittablePolygon::CalculateLocalCentroid()
    {
        for (auto edge : edgesInPolygon)
        {
            localCentroid += edge->position;
        }

        localCentroid /= (float)edgesInPolygon.size();
    }

    void SplittablePolygon::AssignEdgeOwnership()
    {
        for (auto edge : edgesInPolygon)
        {
            edge->owner = weak_from_this();
        }
    }

    void SplittablePolygon::ResetEdgeVisited()
    {
        for (auto edge : edgesInPolygon)
        {
            edge->isVisited = false;
        }
    }

    

    void SplittablePolygon::CalculatePolygonSplit
    (const math::mat4& transform, math::vec3 planePosition, math::vec3 planeNormal)
    {
        int aboveCount = 0;
        int belowCount = 0;

        for (auto edge : edgesInPolygon)
        {
            
            math::vec3 worldPosition = transform * math::vec4(edge->position, 1);

            float distToPlane = PhysicsStatics::PointDistanceToPlane
            (planeNormal, planePosition, worldPosition);

            if (distToPlane > constants::polygonItersectionEpsilon)
            {
                aboveCount++;
            }

            if (distToPlane < constants::polygonItersectionEpsilon)
            {
                belowCount++;
            }
        }

        if (aboveCount > 0 && belowCount == 0)
        {
            m_SplitState = SplitState::Above;
        }
        else if (belowCount > 0 && aboveCount == 0)
        {
            m_SplitState = SplitState::Below;
        }
        else if (belowCount > 0 && aboveCount > 0)
        {
            m_SplitState = SplitState::Split;
        }



    }

    void SplittablePolygon::IdentifyBoundaries(const math::mat4& transform)
    {
        for (auto edge : edgesInPolygon)
        {
            bool edgeIsBoundary = true;
            math::vec3 transformedNormal = edge->CalculateEdgeNormal(transform);
            math::vec3 transformedOtherNormal = edge->pairingEdge->CalculateEdgeNormal(transform);

            if (edge->pairingEdge)
            {
                if (MeshHalfEdge::CompareNormals(transformedNormal,transformedOtherNormal))
                {
                    edgeIsBoundary = false;
                }
            }

            edge->isBoundary = edgeIsBoundary;

        }
    }

    SplitState SplittablePolygon::GetPolygonSplitState() const
    {
        return m_SplitState;
    }

}

