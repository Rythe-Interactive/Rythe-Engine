#pragma once
#include <core/core.hpp>

namespace legion::physics
{
    struct MeshHalfEdge : std::enable_shared_from_this<MeshHalfEdge>
    {
        math::vec3 position;
        std::shared_ptr<MeshHalfEdge> nextEdge;
        std::shared_ptr<MeshHalfEdge> pairingEdge;

        bool isVisited = false;

        MeshHalfEdge(math::vec3 pPosition) : position(pPosition)
        {

        };

        bool AttemptGetTrianglesInEdges
        (std::shared_ptr<MeshHalfEdge>& nextEdge, std::shared_ptr<MeshHalfEdge>& prevEdge)
        {
            if (!this->nextEdge)
            {
                return false;
            }

            nextEdge = this->nextEdge;

            if (!nextEdge->nextEdge)
            {
                return false;
            }

            prevEdge = nextEdge->nextEdge;

            return true;
        }

        void MarkTriangleEdgeVisited()
        {
            isVisited = true;
            nextEdge->isVisited = true;
            nextEdge->nextEdge->isVisited = true;
        }

        void populateQueueWithTriangleNeighbor(std::queue<std::shared_ptr<MeshHalfEdge>>& edgeQueue)
        {
            edgeQueue.push(pairingEdge);
            edgeQueue.push(nextEdge->pairingEdge);
            edgeQueue.push(nextEdge->nextEdge->pairingEdge);
        }

        void populateVectorWithTriangle(std::vector<std::shared_ptr<MeshHalfEdge>>& edgeVector)
        {
            edgeVector.push_back(shared_from_this());
            edgeVector.push_back(nextEdge);
            edgeVector.push_back(nextEdge->nextEdge);
        }

        bool IsTriangleValid()
        {
            std::shared_ptr<MeshHalfEdge> nextEdge = nullptr;
            std::shared_ptr<MeshHalfEdge> prevEdge = nullptr;
            return AttemptGetTrianglesInEdges( nextEdge, prevEdge);
        }

        math::vec3 CalculateEdgeNormal(const math::mat4& transform)
        {
            math::vec3 firstDir = transform * math::vec4(nextEdge->position - position,0);
            math::vec3 secondDir = transform * math::vec4(nextEdge->nextEdge->position - position,0);

            return math::normalize(math::cross(firstDir, secondDir));
        }

        bool IsNormalCloseEnough(const math::vec3& comparisonNormal, const math::mat4& transform)
        {
            static float toleranceResult = 0.05f;

            float dotResult = math::dot(CalculateEdgeNormal(transform), comparisonNormal);

            return math::abs(dotResult) > (1.0f - toleranceResult);
        }

    };


}





