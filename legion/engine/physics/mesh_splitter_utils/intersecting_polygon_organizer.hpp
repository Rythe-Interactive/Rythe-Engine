#pragma once
#include <core/core.hpp>
#include <physics/mesh_splitter_utils/splittable_polygon.h>
#include <physics/mesh_splitter_utils/mesh_splitter_typedefs.hpp>
#include <physics/mesh_splitter_utils/mesh_splitter_debug_helpers.h>
#include <physics/mesh_splitter_utils/intersection_edge_info.h>

namespace legion::physics
{
    /** @struct IntersectingPolygonOrganizer
    * @brief Contains the necessary operations to split the edges of a given polygon
    */
    struct IntersectingPolygonOrganizer
    {
        MeshSplitterDebugHelper* debugHelper = nullptr;

        IntersectingPolygonOrganizer(MeshSplitterDebugHelper* pDebugHelper) : debugHelper(pDebugHelper)
        {

        }

        //------------------------------------------------------ Related to Edge Categorization  ----------------------------------------------------------------//

        void SplitPolygon(SplittablePolygonPtr splitPolygon
            , const math::mat4& transform, const math::vec3 cutPosition
            , const math::vec3 cutNormal, SplitState requestedState
            , std::vector<IntersectionEdgeInfo>& generatedIntersectionEdges)
        {
            BoundaryEdgeInfo polygonDebugInfo;

            bool keepAbove = requestedState == SplitState::Above ? true : false;
            //For now assume no islands in one polygon

            splitPolygon->ResetEdgeVisited();

            if (splitPolygon->GetMeshEdges().size() < 3) { return; }

            //---------------------------- [1] Get Triangles Effected by the split------------------------------------------------------------------//

            std::vector<meshHalfEdgePtr> unEffectedUsedEdges;
            std::vector<meshHalfEdgePtr> effectedUsedEdges;

            BFSIdentifyEffectedEdges(transform, splitPolygon, unEffectedUsedEdges, effectedUsedEdges, cutPosition, cutNormal, keepAbove);

            //---------------------------- [2] Get used boundary Effected ----------------------------------------------------------------------------//

            //set 'effectedUsedEdges' to be unvisited in order to differentiate it from 'unEffectedUsedEdges'
            for (auto edge : effectedUsedEdges)
            {
                edge->isVisited = false;
            }

            IdentifyBoundaryEffectedEdges(effectedUsedEdges,
                transform, cutPosition
                , cutNormal, keepAbove);


            //----------------------------- [3] Sort Edges ---------------------------------------------------------------------------------------//

            //find edges in effectedUsedEdges that are split by plane and store them in a std::vector
            std::vector<meshHalfEdgePtr> splitEdges;

            for (auto edge : effectedUsedEdges)
            {
                if (edge->IsSplitByPlane(transform, cutPosition, cutNormal))
                {
                    splitEdges.push_back(edge);
                }
            }

            //sort them based on support point created by cross product of normal and split normal

            math::vec3 worldCentroid = transform * math::vec4(splitPolygon->localCentroid, 1);
            math::vec3 worldPolygonNormal = transform * math::vec4(splitPolygon->localNormal, 0);
            polygonDebugInfo.worldNormal = worldPolygonNormal;

            math::vec3 polygonNormalCrossCutNormal = math::normalize(math::cross(worldPolygonNormal, cutNormal));

            //SortingCriterium splittingPlaneSorter(worldCentroid,polygonNormalCrossCutNormal,transform);

            auto initialSorter = [&transform, &worldCentroid, &polygonNormalCrossCutNormal]
            (const std::shared_ptr<MeshHalfEdge>& lhs, std::shared_ptr<MeshHalfEdge>& rhs)
            {
                math::vec3 aWorldCentroid = lhs->GetWorldCentroid(transform);
                math::vec3 bWorldCentroid = rhs->GetWorldCentroid(transform);

                math::vec3 AtoPolygonCentroid = aWorldCentroid - worldCentroid;
                math::vec3 BtoPolygonCentroid = bWorldCentroid - worldCentroid;

                return
                    math::dot(AtoPolygonCentroid, polygonNormalCrossCutNormal) <
                    math::dot(BtoPolygonCentroid, polygonNormalCrossCutNormal);


            };


            std::sort(splitEdges.begin(), splitEdges.end(), initialSorter);

            //clear the polygon's edge list because we are going to start regenerating it
            auto& meshEdges = splitPolygon->GetMeshEdges();
            meshEdges.clear();

            int edgesUsed = effectedUsedEdges.size();
            static int numberOfEdgesToMakeATriangle = 2;

            //Check for the special case where there are only 2 splitEdges, this will create 1 triangle
            if (edgesUsed == numberOfEdgesToMakeATriangle)
            {
                HandleTriangleSplit(splitPolygon,
                    splitEdges, generatedIntersectionEdges,
                    transform, cutNormal, cutPosition, keepAbove);

                return;
            }
            else if(edgesUsed < numberOfEdgesToMakeATriangle)
            {
                //there exist a number of edges that is on the splitting plane
                assert(false);
                return;
            }

            //previous sorting direction is dependent on cut tangent.
            //use centroids of first and last split edge as the new sorting direction

            math::vec3 worldFirstEdge = splitEdges.at(0)->GetWorldCentroid(transform);
            math::vec3 worldSecondEdge = splitEdges.at(splitEdges.size() - 1)->GetWorldCentroid(transform);

            //sort effectedUsedEdges based on sorting direction

            math::vec3 sortingCentroid = (worldFirstEdge + worldSecondEdge) / 2.0f;
            math::vec3 sortingDirection = worldSecondEdge - worldFirstEdge;

            auto boundarySorter = [&transform, &sortingCentroid, &sortingDirection]
            (const std::shared_ptr<MeshHalfEdge>& lhs, std::shared_ptr<MeshHalfEdge>& rhs)
            {
                math::vec3 aWorldCentroid = lhs->GetWorldCentroid(transform);
                math::vec3 bWorldCentroid = rhs->GetWorldCentroid(transform);

                math::vec3 AtoPolygonCentroid = aWorldCentroid - sortingCentroid;
                math::vec3 BtoPolygonCentroid = bWorldCentroid - sortingCentroid;

                return
                    math::dot(AtoPolygonCentroid, sortingDirection) <
                    math::dot(BtoPolygonCentroid, sortingDirection);

            };

            std::sort(effectedUsedEdges.begin(), effectedUsedEdges.end(), boundarySorter);

            meshHalfEdgePtr firstEdge = effectedUsedEdges.at(0);
            meshHalfEdgePtr secondEdge = effectedUsedEdges.at(effectedUsedEdges.size() - 1);


            //------------------------------------- [5] Regenerate Edges ----------------------------------------------------------------------------------------------------------//


            //get start and end intersection points
            auto [firstEdgeCurrent, firstEdgeNext] = firstEdge->GetEdgeWorldPositions(transform);

            //check if position of first edge is outside intersection, this is important because it will determine how the edges will be regenerated
            bool startFromOutsideIntersection =
                keepAbove == PhysicsStatics::IsPointAbovePlane(cutNormal, cutPosition, firstEdgeCurrent);

            auto [firstEdgeIntersection, firstInterpolantUV, secondEdgeIntersection, secondInterpolantUV]
                = GetFirstAndLastEdgeIntersectionInfo(cutNormal, cutPosition, transform,
                firstEdge, secondEdge);

            math::vec3 startToEndIntersection = secondEdgeIntersection - firstEdgeIntersection;
            math::vec2 startToEndUV = secondInterpolantUV - firstInterpolantUV;

            meshHalfEdgePtr supportEdge = nullptr;
            //generated half edges using
            std::vector<meshHalfEdgePtr> generatedHalfEdges;

            SplitterIntersectionInfo vertexIntersectionInfo(
                firstEdgeIntersection,
                startToEndIntersection,
                firstInterpolantUV,
                startToEndUV);

            for (int i = 1; i < effectedUsedEdges.size() - 1; i++)
            {
                if (startFromOutsideIntersection)
                {
                    InsideIntersectionMeshRegeneration(
                        transform, effectedUsedEdges,
                        generatedHalfEdges, i, supportEdge, vertexIntersectionInfo, polygonDebugInfo, generatedIntersectionEdges,splitPolygon);
                }
                else
                {
                    OutsideIntersectionMeshRegeneration(
                        transform, effectedUsedEdges,
                        generatedHalfEdges, i, supportEdge, vertexIntersectionInfo, polygonDebugInfo, generatedIntersectionEdges, splitPolygon);
                }
            }

            polygonDebugInfo.intersectionPoints.first = firstEdgeIntersection;
            polygonDebugInfo.intersectionPoints.second = secondEdgeIntersection;

            polygonDebugInfo.finalSortingDirection.first = sortingCentroid;
            polygonDebugInfo.finalSortingDirection.second = sortingDirection;

            polygonDebugInfo.drawColor = splitPolygon->debugColor;

            //--------------DEBUG STUFF----------------//
            polygonDebugInfo.boundaryEdges.insert(
                polygonDebugInfo.boundaryEdges.begin(),
                effectedUsedEdges.begin(),
                effectedUsedEdges.end());

            polygonDebugInfo.boundaryEdges.insert(
                polygonDebugInfo.boundaryEdges.end(),
                generatedHalfEdges.begin(),
                generatedHalfEdges.end());
            //-------------------------------------//

            debugHelper->boundaryEdgesForPolygon.push_back(polygonDebugInfo);

            //------------------------------------- [6] Add regenerated edges to the polygon -----------------------------------------------------------------------------------//

            meshEdges.insert(meshEdges.end(), unEffectedUsedEdges.begin(), unEffectedUsedEdges.end());
            meshEdges.insert(meshEdges.end(), effectedUsedEdges.begin(), effectedUsedEdges.end());
            meshEdges.insert(meshEdges.end(), generatedHalfEdges.begin(), generatedHalfEdges.end());

            splitPolygon->CalculateLocalCentroid();
            splitPolygon->AssignEdgeOwnership();
            debugHelper->polygonCount++;
        }

        //------------------------------------------------------ Edge Categorization Helper Functions ----------------------------------------------------------------//

        std::tuple<math::vec3, math::vec2, math::vec3, math::vec2> GetFirstAndLastEdgeIntersectionInfo(const math::vec3& cutNormal, const math::vec3& cutPosition
            , const math::mat4 transform,
            meshHalfEdgePtr firstEdge, meshHalfEdgePtr secondEdge)
        {
            auto [firstEdgeCurrent, firstEdgeNext] = firstEdge->GetEdgeWorldPositions(transform);
            auto [secondEdgeCurrent, secondEdgeNext] = secondEdge->GetEdgeWorldPositions(transform);

            math::vec3 firstEdgeIntersection;
            float firstEdgeInterpolant;
            PhysicsStatics::FindLineToPlaneIntersectionPoint(cutNormal, cutPosition,
                firstEdgeCurrent, firstEdgeNext, firstEdgeIntersection, firstEdgeInterpolant);

            math::vec2 firstInterpolantUV = math::lerp(firstEdge->uv, firstEdge->nextEdge->uv, firstEdgeInterpolant);

            math::vec3 secondEdgeIntersection;
            float secondEdgeInterpolant;
            PhysicsStatics::FindLineToPlaneIntersectionPoint(cutNormal, cutPosition,
                secondEdgeCurrent, secondEdgeNext, secondEdgeIntersection, secondEdgeInterpolant);

            math::vec2 secondInterpolantUV = math::lerp(secondEdge->uv, secondEdge->nextEdge->uv, secondEdgeInterpolant);

            math::vec3 startToEndIntersection = secondEdgeIntersection - firstEdgeIntersection;
            math::vec2 startToEndUV = secondInterpolantUV - firstInterpolantUV;

            return std::make_tuple(firstEdgeIntersection, firstInterpolantUV, secondEdgeIntersection, secondInterpolantUV);
        }

        void RegenerateTriangleSplit(bool startFromOutsideIntersection
            , meshHalfEdgePtr firstSplitEdge, meshHalfEdgePtr secondSplitEdge
            , meshHalfEdgePtr& intersectionEdge, const math::vec3& cutNormal, const math::vec3& cutPosition,
            const math::mat4& transform)
        {
            auto [firstEdgeIntersection, firstInterpolantUV, secondEdgeIntersection, secondInterpolantUV]
                = GetFirstAndLastEdgeIntersectionInfo(cutNormal, cutPosition, transform,
                    firstSplitEdge, secondSplitEdge);

            math::vec3 startToEndIntersection = secondEdgeIntersection - firstEdgeIntersection;
            math::vec2 startToEndUV = secondInterpolantUV - firstInterpolantUV;

            const math::mat4 inverseTrans{ math::inverse(transform) };

            if (!startFromOutsideIntersection)
            {
                firstSplitEdge->position = inverseTrans * math::vec4(firstEdgeIntersection, 1);
                firstSplitEdge->uv = firstInterpolantUV;

                intersectionEdge = std::make_shared<MeshHalfEdge>
                    (inverseTrans * math::vec4(secondEdgeIntersection, 1), secondInterpolantUV);

                MeshHalfEdge::ConnectIntoTriangle(firstSplitEdge, secondSplitEdge, intersectionEdge);
            }
            else
            {
                intersectionEdge = std::make_shared<MeshHalfEdge>
                    (inverseTrans * math::vec4(firstEdgeIntersection, 1), firstInterpolantUV);

                secondSplitEdge->position = inverseTrans * math::vec4(secondEdgeIntersection, 1);
                secondSplitEdge->uv = secondInterpolantUV;

                MeshHalfEdge::ConnectIntoTriangle(firstSplitEdge, intersectionEdge, secondSplitEdge);
            }
        }

        void BFSIdentifyEffectedEdges(const math::mat4& transform, SplittablePolygonPtr splitPolygon,
            std::vector<meshHalfEdgePtr>& unEffectedUsedEdges
            , std::vector<meshHalfEdgePtr>& effectedUsedEdges
            , const math::vec3 cutPosition,const math::vec3 cutNormal,bool keepAbove)
        {
            meshHalfEdgePtr initalEdge = splitPolygon->GetMeshEdges().at(0);
            //intialize unvisited queue
            std::queue<meshHalfEdgePtr> unvisitedEdges;
            unvisitedEdges.push(initalEdge);


            while (!unvisitedEdges.empty())
            {
                auto currentEdge = unvisitedEdges.front();
                unvisitedEdges.pop();

                if (!currentEdge->isVisited)
                {
                    currentEdge->MarkTriangleEdgeVisited();

                    auto [edge1, edge2, edge3] = currentEdge->GetTriangle();

                    bool edge1Split = edge1->IsSplitByPlane(transform, cutPosition, cutNormal);
                    bool edge2Split = edge2->IsSplitByPlane(transform, cutPosition, cutNormal);
                    bool edge3Split = edge3->IsSplitByPlane(transform, cutPosition, cutNormal);

                    if (edge1Split || edge2Split || edge3Split)
                    {
                        currentEdge->populateVectorWithTriangle(effectedUsedEdges);
                    }
                    else
                    {
                        //if we reach this point the triangle is either completely above the plane or completly below it
                        //we cache it only if its at the correct position

                        math::vec3 edgePosition = transform * math::vec4(currentEdge->position, 1);

                        if (keepAbove == PhysicsStatics::IsPointAbovePlane(cutNormal, cutPosition, edgePosition))
                        {
                            currentEdge->populateVectorWithTriangle(unEffectedUsedEdges);
                        }

                    }

                    if (!edge1->isBoundary)
                    {
                        unvisitedEdges.push(edge1->pairingEdge);
                    }

                    if (!edge2->isBoundary)
                    {
                        unvisitedEdges.push(edge2->pairingEdge);
                    }

                    if (!edge3->isBoundary)
                    {
                        unvisitedEdges.push(edge3->pairingEdge);
                    }
                }
            }

        }

        void IdentifyBoundaryEffectedEdges( std::vector<meshHalfEdgePtr>& effectedUsedEdges,
            const math::mat4 transform, const math::vec3 cutPosition
            , const math::vec3 cutNormal, bool keepAbove)
        {
            std::vector<meshHalfEdgePtr> tempEdges{ std::move(effectedUsedEdges) };
            effectedUsedEdges.clear();

            for (auto edge : tempEdges)
            {
                bool isAtLeastPartiallyAtRequestedSpot = keepAbove ?
                    edge->isEdgePartlyAbovePlane(transform, cutPosition, cutNormal)
                    : edge->isEdgePartlyBelowPlane(transform, cutPosition, cutNormal);

                bool isBoundaryEffectedOrPolygonBoundary = edge->isBoundary || edge->pairingEdge->isVisited;

                if (isAtLeastPartiallyAtRequestedSpot && isBoundaryEffectedOrPolygonBoundary)
                {
                    effectedUsedEdges.push_back(std::move(edge));
                }

            }

            //tempEdges gets destroyed at end of scope
        }

        void HandleTriangleSplit(SplittablePolygonPtr splitPolygon,
            std::vector<meshHalfEdgePtr>& splitEdges, std::vector<IntersectionEdgeInfo>& generatedIntersectionEdges,
            const math::mat4& transform, const math::vec3& cutNormal,const math::vec3& cutPosition, bool keepAbove)
        {
            auto& meshEdges = splitPolygon->GetMeshEdges();

            meshHalfEdgePtr firstEdge = splitEdges.at(0);
            meshHalfEdgePtr secondEdge = splitEdges.at(1);

            math::vec3 edgeWorldPosition = firstEdge->GetEdgeWorldPosition(transform);

            bool startFromOutsideIntersection =
                keepAbove == PhysicsStatics::IsPointAbovePlane(cutNormal, cutPosition, edgeWorldPosition);

            //handle triangle split
            meshHalfEdgePtr intersectionEdge = nullptr;

            RegenerateTriangleSplit(startFromOutsideIntersection
                , firstEdge, secondEdge,
                intersectionEdge, cutNormal, cutPosition,
                transform);

            IntersectionEdgeInfo intersectionEdgeInfo(intersectionEdge);

            generatedIntersectionEdges.push_back(intersectionEdgeInfo);


            meshEdges.push_back(firstEdge);
            meshEdges.push_back(secondEdge);
            meshEdges.push_back(intersectionEdge);

            splitPolygon->CalculateLocalCentroid();
            splitPolygon->AssignEdgeOwnership();
        }


        //------------------------------------------------------ Related to Quad Organization ----------------------------------------------------------------//

        void InsideIntersectionMeshRegeneration(const math::mat4& transform, std::vector<meshHalfEdgePtr>& effectedBoundaryEdges,
            std::vector<meshHalfEdgePtr>& generatedEdges, int i
            , meshHalfEdgePtr& supportEdge,
            const SplitterIntersectionInfo& vertexIntersectionInfo, BoundaryEdgeInfo& debugStuff, std::vector<IntersectionEdgeInfo>& generatedIntersectionEdges
            , SplittablePolygonPtr owner)
        {
            //select Base Half Edge
            meshHalfEdgePtr baseEdge = effectedBoundaryEdges[i];
            debugStuff.base = baseEdge->GetEdgeWorldPosition(transform);


            /*const math::vec3& initialIntersectionPosition = vertexIntersectionInfo.startIntersectionPosition;
            const math::vec3& startToEndPosition = vertexIntersectionInfo.startToEndPosition;
            const math::vec2& initialUVIntersection = vertexIntersectionInfo.startIntersectionUV;
            const math::vec2& startToEndUV = vertexIntersectionInfo.startToEndUV;*/

            auto [initialIntersectionPosition, startToEndPosition,
                initialUVIntersection, startToEndUV] = vertexIntersectionInfo.GetIntersectionData();

            //----------------------------------- Create currentSupportEdge -------------------------------------------------------------//
            meshHalfEdgePtr currentSupportEdge = nullptr;

            if (i == 1)
            {
                currentSupportEdge = effectedBoundaryEdges[0];
                //currentSupportEdge.position = transform.worldToLocalMatrix.MultiplyPoint(worldStartIntersection);
            }
            else
            {
                currentSupportEdge = std::make_shared<MeshHalfEdge>
                    (baseEdge->nextEdge->position, baseEdge->nextEdge->uv, owner);
               

                currentSupportEdge->SetPairing(supportEdge);
                generatedEdges.push_back(currentSupportEdge);

            }

            debugStuff.prevSupport = currentSupportEdge->GetEdgeWorldPosition(transform);
            //----------------------------------- Create nextcurrentSupportEdge -------------------------------------------------------------//
            int maxData = effectedBoundaryEdges.size() - 1;
            meshHalfEdgePtr nextSupportEdge;

            if (i + 1 == effectedBoundaryEdges.size() - 1)
            {
                nextSupportEdge = effectedBoundaryEdges[effectedBoundaryEdges.size() - 1];

                math::vec3 worldPosition =
                    (initialIntersectionPosition + startToEndPosition);

                nextSupportEdge->position = math::inverse(transform) * math::vec4((worldPosition), 1);
                nextSupportEdge->uv = initialUVIntersection + startToEndUV;
            }
            else
            {
                math::vec3 worldPosition = initialIntersectionPosition
                    + startToEndPosition * (float)i / maxData;

                math::vec2 uv = initialUVIntersection + startToEndUV * (float)i / maxData;

                nextSupportEdge = std::make_shared< MeshHalfEdge>(
                    math::inverse(transform) * math::vec4(worldPosition, 1), uv, owner);

                generatedEdges.push_back(nextSupportEdge);
            }

            debugStuff.nextSupport = nextSupportEdge->GetEdgeWorldPosition(transform);

            //----------------------------------- Create Intersection -------------------------------------------------------------//

            int currentIndex = i - 1;


            //i - 1 < 0 ? 0 : i - 1 ;
            float interpolant = (float)currentIndex / maxData;

            math::vec3 worldIntersectionPosition =
                initialIntersectionPosition + (startToEndPosition * interpolant);

            math::vec3 localIntersectionPosition = math::inverse(transform)
                * math::vec4(worldIntersectionPosition, 1);

            math::vec2 uv = initialUVIntersection + startToEndUV * interpolant;

            meshHalfEdgePtr intersectionEdge = std::make_shared< MeshHalfEdge>(
                localIntersectionPosition, uv, owner);


            generatedEdges.push_back(intersectionEdge);
            intersectionEdge->isBoundary = true;

            debugStuff.intersectionEdge = intersectionEdge->GetEdgeWorldPosition(transform);

            CreateNonAllignedQuad(currentSupportEdge, nextSupportEdge, baseEdge, intersectionEdge, generatedEdges,owner);

            //-------------------------- Insantiate intersection info ----------------------------------------------//

            IntersectionEdgeInfo intersectionEdgeInfo(
                intersectionEdge);
            generatedIntersectionEdges.push_back(intersectionEdgeInfo);
            

            supportEdge = nextSupportEdge;



        }

        void CreateNonAllignedQuad(
            meshHalfEdgePtr currentSupport, meshHalfEdgePtr nextSupport, meshHalfEdgePtr baseEdge, meshHalfEdgePtr intersectionEdge,
            std::vector<meshHalfEdgePtr>& generatedEdges, SplittablePolygonPtr owner
        )
        {
            //create new supporttriangle located at next support
            meshHalfEdgePtr supportTriangle = std::make_shared< MeshHalfEdge>(nextSupport->position, nextSupport->uv, owner);


            //currentSupport-intersection-supporttriangle
            MeshHalfEdge::ConnectIntoTriangle(currentSupport, intersectionEdge, supportTriangle);

            //create new nextsupporttriangle located at currentsupport
            meshHalfEdgePtr nextSupportTriangle = std::make_shared< MeshHalfEdge>(currentSupport->position, currentSupport->uv, owner);

            //nextsupporttriangle-nextSupport-baseEdge
            MeshHalfEdge::ConnectIntoTriangle(nextSupportTriangle, nextSupport, baseEdge);

            supportTriangle->SetPairing(nextSupportTriangle);
            generatedEdges.push_back(supportTriangle);
            generatedEdges.push_back(nextSupportTriangle);


        }

        void OutsideIntersectionMeshRegeneration(const math::mat4& transform, std::vector<meshHalfEdgePtr>& effectedBoundaryEdges,
            std::vector<meshHalfEdgePtr>& generatedEdges, int i
            , meshHalfEdgePtr supportEdge,
            const SplitterIntersectionInfo& vertexIntersectionInfo, BoundaryEdgeInfo& debugStuff, std::vector<IntersectionEdgeInfo>& generatedIntersectionEdges
            , SplittablePolygonPtr owner)
        {
            meshHalfEdgePtr baseEdge = effectedBoundaryEdges[i];

            auto [initialIntersectionPosition, startToEndPosition,
                initialUVIntersection, startToEndUV] = vertexIntersectionInfo.GetIntersectionData();

            //----------------------------------- Create currentSupportEdge -------------------------------------------------------------//
            meshHalfEdgePtr currentSupportEdge;
            if (i == 1)
            {
                currentSupportEdge = effectedBoundaryEdges[0];

                currentSupportEdge->position = math::inverse(transform) *
                    math::vec4(initialIntersectionPosition, 1);

                currentSupportEdge->uv = initialUVIntersection;

            }
            else
            {
                currentSupportEdge = std::make_shared<MeshHalfEdge>
                    (supportEdge->nextEdge->position, supportEdge->nextEdge->uv, owner);

                currentSupportEdge->SetPairing(supportEdge);
                generatedEdges.push_back(currentSupportEdge);
            }


            //----------------------------------- Create nextcurrentSupportEdge -------------------------------------------------------------//
            meshHalfEdgePtr  nextSupportEdge = nullptr;
            if (i + 1 == effectedBoundaryEdges.size() - 1)
            {
                nextSupportEdge = effectedBoundaryEdges[effectedBoundaryEdges.size() - 1];
                //currentSupportEdge.position = transform.worldToLocalMatrix.MultiplyPoint(worldStartIntersection + startToEndIntersection);
            }
            else
            {
                nextSupportEdge = std::make_shared<MeshHalfEdge>(baseEdge->nextEdge->position, baseEdge->nextEdge->uv, owner);
                generatedEdges.push_back(nextSupportEdge);
            }

            supportEdge = nextSupportEdge;
            //----------------------------------- Create Intersection -------------------------------------------------------------//
            //Vector3 intersectionPosition;
            int maxData = effectedBoundaryEdges.size() - 1;
            int currentIndex = i + 1;
            float interpolant = (float)currentIndex / maxData;

            math::vec3 IntersectionEdgePosition =
                initialIntersectionPosition + (startToEndPosition * interpolant);

            math::vec3 localIntersectionEdgePosition =
                math::inverse(transform) * math::vec4(IntersectionEdgePosition, 1);

            math::vec2 edgeUV = initialUVIntersection + startToEndUV * interpolant;

            meshHalfEdgePtr intersectionEdge = std::make_shared<MeshHalfEdge>
                (localIntersectionEdgePosition, edgeUV, owner);
            intersectionEdge->isBoundary = true;
            generatedEdges.push_back(intersectionEdge);

            CreateAllignedQuad(currentSupportEdge, nextSupportEdge, baseEdge,
                intersectionEdge, generatedEdges, owner);

            IntersectionEdgeInfo intersectionEdgeInfo(
                intersectionEdge);

            generatedIntersectionEdges.push_back(intersectionEdgeInfo);

            

        }

        void CreateAllignedQuad(meshHalfEdgePtr currentSupport, meshHalfEdgePtr nextSupport, meshHalfEdgePtr baseEdge,
            meshHalfEdgePtr intersectionEdge, std::vector<meshHalfEdgePtr>& generatedEdges, SplittablePolygonPtr owner)
        {
            //create new supporttriangle located at next support
            meshHalfEdgePtr supportTriangle = std::make_shared<MeshHalfEdge>(nextSupport->position, nextSupport->uv, owner);

            //currentSupport-intersection-supporttriangle
            MeshHalfEdge::ConnectIntoTriangle(currentSupport, supportTriangle, intersectionEdge);

            //create new nextsupporttriangle located at currentsupport
            meshHalfEdgePtr  nextSupportTriangle = std::make_shared<MeshHalfEdge>(currentSupport->position, currentSupport->uv, owner);

            //nextsupporttriangle-nextSupport-baseEdge
            MeshHalfEdge::ConnectIntoTriangle(nextSupportTriangle, baseEdge, nextSupport);

            supportTriangle->SetPairing(nextSupportTriangle);

            generatedEdges.push_back(supportTriangle);
            generatedEdges.push_back(nextSupportTriangle);
        }

    };





}
