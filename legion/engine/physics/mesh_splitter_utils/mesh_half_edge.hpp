#pragma once
#include <core/core.hpp>
#include <physics/mesh_splitter_utils/splittable_polygon.h>
#include <physics/physics_statics.hpp>

namespace legion::physics
{
    struct MeshHalfEdge : std::enable_shared_from_this<MeshHalfEdge>
    {
        math::vec3 position;
        math::vec2 uv;

        std::shared_ptr<MeshHalfEdge> nextEdge = nullptr;
        std::shared_ptr<MeshHalfEdge> pairingEdge = nullptr;
        std::shared_ptr<MeshHalfEdge> shadowEdge = nullptr;

        std::weak_ptr<SplittablePolygon> owner;

        //TODO: these bools should probably be stored as an array in SplittablePolygon
        bool isVisited = false;
        bool isBoundary = false;

        //std::string debugID = "";

        MeshHalfEdge(const math::vec3& pPosition, const math::vec2& pUVs, std::weak_ptr<SplittablePolygon> pOwner) : position(pPosition), uv(pUVs),owner(pOwner)
        {

        }

        MeshHalfEdge(const math::vec3& pPosition,const math::vec2& pUVs) : position(pPosition),uv(pUVs)
        {

        }

        MeshHalfEdge(const math::vec3& pPosition) : position(pPosition)
        {

        }

        auto GetTriangle() 
        {
            return std::make_tuple(shared_from_this(), nextEdge, nextEdge->nextEdge);
        }

        auto GetShadowTriangle()
        {
            return std::make_tuple(shadowEdge, nextEdge->shadowEdge, nextEdge->nextEdge->shadowEdge);
        }

        void SetPairing(std::shared_ptr<MeshHalfEdge>& newPairing)
        {
            pairingEdge = newPairing;
            newPairing->pairingEdge = shared_from_this();
        }

        void CloneOnShadowEdge()
        {
            shadowEdge = std::make_shared<MeshHalfEdge>(position, uv);
            shadowEdge->isBoundary = isBoundary;
        }

        static void ConnectIntoTriangle(std::shared_ptr<MeshHalfEdge> first
            , std::shared_ptr<MeshHalfEdge> second,
            std::shared_ptr<MeshHalfEdge> third)
        {
            first->nextEdge = second;
            second->nextEdge = third;
            third->nextEdge = first;
        }
 

        bool IsSplitByPlane(const math::mat4& transform
            , const math::vec3& planePosition
            ,const  math::vec3& planeNormal)
        {
            int x = 0;
            int y = 0;

            auto [currentDistFromPlane, nextDistFromPlane] = GetEdgeDistancesFromPlane(transform, planePosition, planeNormal);

            if (currentDistFromPlane > constants::polygonSplitterEpsilon)
            {
                x = 1;
            }
            else if (currentDistFromPlane < -constants::polygonSplitterEpsilon)
            {
                x = -1;
            }


            if (nextDistFromPlane > constants::polygonSplitterEpsilon)
            {
                y = 1;
            }
            else if (nextDistFromPlane < -constants::polygonSplitterEpsilon)
            {
                y = -1;
            }

            int edgeSplitState = x * y;


            return edgeSplitState < 0;


        }

        math::vec3 GetWorldCentroid(const math::mat4& transform) const
        {
            return transform * math::vec4((position + nextEdge->position) * 0.5f, 1);
        }

        bool isEdgePartlyAbovePlane(const math::mat4& transform, const math::vec3& planePosition, const math::vec3& planeNormal)
        {
            auto [currentDistFromPlane, nextDistFromPlane] = GetEdgeDistancesFromPlane(transform, planePosition, planeNormal);

            bool currentAbovePlane = currentDistFromPlane > constants::polygonSplitterEpsilon;
            bool nextAbovePlane = nextDistFromPlane > constants::polygonSplitterEpsilon;

            return currentAbovePlane || nextAbovePlane;
        }

        bool isEdgePartlyBelowPlane(const math::mat4& transform, const math::vec3& planePosition, const math::vec3& planeNormal)
        {
            auto [currentDistFromPlane, nextDistFromPlane] = GetEdgeDistancesFromPlane(transform, planePosition, planeNormal);

            bool currentBelowPlane = currentDistFromPlane < -constants::polygonSplitterEpsilon;
            bool nextBelowPlane = nextDistFromPlane < -constants::polygonSplitterEpsilon;

            return currentBelowPlane || nextBelowPlane;
        }

        std::tuple<float, float> GetEdgeDistancesFromPlane(const math::mat4& transform, const math::vec3& planePosition, const math::vec3& planeNormal)
        {
            auto [currentWorldPos, nextWorldPos] = GetEdgeWorldPositions(transform);

            float currentDistFromPlane = PhysicsStatics::PointDistanceToPlane(planeNormal, planePosition,  currentWorldPos);
            float nextDistFromPlane = PhysicsStatics::PointDistanceToPlane(planeNormal, planePosition,  nextWorldPos);

            return std::make_tuple(currentDistFromPlane, nextDistFromPlane);
        }

        std::tuple<math::vec3, math::vec3> GetEdgeWorldPositions(const math::mat4& transform)
        {
            math::vec3 currentWorldPos = GetEdgeWorldPosition(transform);
            math::vec3 nextWorldPos = nextEdge->GetEdgeWorldPosition(transform);

            return std::make_tuple(currentWorldPos, nextWorldPos);
        }

        math::vec3 GetEdgeWorldPosition(const math::mat4& transform)
        {
            return transform * math::vec4(position, 1);
        }



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
            if (pairingEdge)
            {
                edgeQueue.push(pairingEdge);
            }
            //else
            //{
            //    isBoundary = true;
            //}


            if (nextEdge->pairingEdge)
            {
                edgeQueue.push(nextEdge->pairingEdge);
            }
            /*else
            {
                nextEdge->isBoundary = true;
            }*/

            if (nextEdge->nextEdge->pairingEdge)
            {
                edgeQueue.push(nextEdge->nextEdge->pairingEdge);
            }
            /*else
            {
                nextEdge->nextEdge->isBoundary = true;
            }*/

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
            
            //float dotResult = math::dot(CalculateEdgeNormal(transform), comparisonNormal);

            return CompareNormals(CalculateEdgeNormal(transform), comparisonNormal);
        }

        static bool CompareNormals(const math::vec3& comparisonNormal, const math::vec3& otherNormal) 
        {
            static float toleranceDot = math::cos(math::deg2rad(5.0f));

            float dotResult = math::dot(otherNormal, comparisonNormal);


            return dotResult > toleranceDot;
        }

        


    };


}





