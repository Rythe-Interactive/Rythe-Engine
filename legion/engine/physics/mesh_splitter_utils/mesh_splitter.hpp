#pragma once

#include <core/core.hpp>
#include <physics/mesh_splitter_utils/mesh_splitter_typedefs.hpp>
#include <physics/mesh_splitter_utils/mesh_half_edge.hpp>
#include <physics/mesh_splitter_utils/splittable_polygon.h>
#include <rendering/components/renderable.hpp>
#include <physics/mesh_splitter_utils/primitive_mesh.h>
#include <rendering/components/renderable.hpp>
#include <physics/mesh_splitter_utils/half_edge_finder.h>
#include <physics/mesh_splitter_utils/mesh_splitter_debug_helpers.h>

namespace legion::physics
{
    struct MeshSplitter
    {
        HalfEdgeFinder edgeFinder;
        ecs::entity_handle owner;
        ecs::entity_handle splitTester;

        rendering::material_handle ownerMaterialH;
        
        std::vector<SplittablePolygonPtr> meshPolygons;

        MeshSplitterDebugHelper debugHelper;

        void TestSplit()
        {
            
            if (splitTester)
            {
                
                auto [posH, rotH, scaleH] = splitTester.get_component_handles<transform>();
                const math::mat4 transform = math::compose(scaleH.read(), rotH.read(), posH.read());

                const math::vec3 worldUp = transform * math::vec4(0, 1, 0,0);

                SplitMesh(posH.read(), math::normalize(worldUp), true);

                
            }
            else
            {
                log::error("Split tester not set");
            }
        }

        void DestroyTestSplitter(ecs::EcsRegistry* m_ecs)
        {
            m_ecs->destroyEntity(splitTester);
        }

        /** @brief Creates a Half-Edge Data structure around the mesh and
        * @param entity the entity that this MeshSplitter is attached to
        */
        void InitializePolygons(ecs::entity_handle entity)
        {
            owner = entity;

            auto [meshFilter,meshRenderer] = entity.get_component_handles<rendering::renderable>();

            ownerMaterialH = meshRenderer.read().material;

            auto [posH, rotH, scaleH] = entity.get_component_handles<transform>();
            
            if (meshFilter && posH && rotH && scaleH)
            {
                log::debug("Mesh and Transform found");
                std::queue<meshHalfEdgePtr> meshHalfEdges;

                //auto renderable = renderable.read();
                mesh& mesh = meshFilter.read().get().second;

                const math::mat4 transform = math::compose(scaleH.read(), rotH.read(), posH.read());

                edgeFinder.FindHalfEdge(mesh, transform, meshHalfEdges);

                BFSPolygonize(meshHalfEdges, transform);

                log::debug("Mesh vertices {}, Mesh indices {}", mesh.vertices.size(), mesh.indices.size());


            }
            else
            {
                log::warn("The given entity does not have a meshHandle!");
            }

        }

        /** @brief Given a queue of edges and a transform,
        * populates the std::vector 'meshPolygons' using BFS
        */
        void BFSPolygonize(std::queue<meshHalfEdgePtr>& halfEdgeQueue, const math::mat4& transform)
        {
            //while edge queue is not empty
            while (!halfEdgeQueue.empty())
            {
                meshHalfEdgePtr startEdge = halfEdgeQueue.front();
                halfEdgeQueue.pop();

                if (!startEdge->isVisited)
                {
                    SplittablePolygonPtr polygon = nullptr;

                    if (BFSIdentifyPolygon(startEdge, polygon, halfEdgeQueue, transform))
                    {
                        meshPolygons.push_back(polygon);
                    }
                }
                //break;
            }


        }

        /** @brief Given an intial startEdge, do a BFS to identify the polygon
        * that the edge is in. Also populates the halfEdgeQueue with the
        */
        bool BFSIdentifyPolygon(meshHalfEdgePtr startEdge
            , std::shared_ptr<SplittablePolygon>& polygon, std::queue<meshHalfEdgePtr>& halfEdgeQueue
            , const math::mat4& transform)
        {
            log::debug("->BFSIdentifyPolygon");
            //polygonEdgeList : edges considered to be in the same polygon
            std::vector<meshHalfEdgePtr> edgesInPolygon;

            meshHalfEdgePtr nextEdge = nullptr;
            meshHalfEdgePtr prevEdge = nullptr;

            //startEdge may not form a triangle,we early out if this happens
            if (!startEdge->AttemptGetTrianglesInEdges(nextEdge, prevEdge))
            {
                return false;
            }

            assert(nextEdge);
            assert(prevEdge);

            edgesInPolygon.push_back(startEdge);
            edgesInPolygon.push_back(nextEdge);
            edgesInPolygon.push_back(prevEdge);

            //mark all edges visited
            startEdge->MarkTriangleEdgeVisited();

            //get all neigbors of the startEdge Triangle and put them in unvisitedEdgeQueue
            std::queue<meshHalfEdgePtr> unvisitedEdgeQueue;
            startEdge->populateQueueWithTriangleNeighbor(unvisitedEdgeQueue);

            std::vector<meshHalfEdgePtr> edgesNotInPolygon;

            const math::vec3 comparisonNormal = startEdge->CalculateEdgeNormal(transform);

            //BFS search for adjacent triangles with same normal
            while (!unvisitedEdgeQueue.empty())
            {
                //log::debug("Pop Edge");

                auto edgeToCheck = unvisitedEdgeQueue.front();
                unvisitedEdgeQueue.pop();

                if (!edgeToCheck) { continue; }

                if (!edgeToCheck->isVisited && edgeToCheck->IsTriangleValid())
                {
                    edgeToCheck->MarkTriangleEdgeVisited();

                    //if triangle has same normal as original
                    if (edgeToCheck->IsNormalCloseEnough(comparisonNormal, transform))
                    {
                        //add all edges in triangle to polygonEdgeList
                        edgeToCheck->populateVectorWithTriangle(edgesInPolygon);
                        //add neigbors to polygonEdgeNonVisitedQueue
                        edgeToCheck->populateQueueWithTriangleNeighbor(unvisitedEdgeQueue);
                        //log::debug("Is in same polygon");
                    }
                    else
                    {
                        //add edge to edgesNotInPolygon
                        edgeToCheck->populateVectorWithTriangle(edgesNotInPolygon);
                        //log::debug("Is NOT same polygon");
                        //edgeToCheck->pairingEdge->isBoundary = true;
                    }


                }

            }

            for (auto edge : edgesNotInPolygon)
            {
                edge->isVisited = false;
                halfEdgeQueue.push(edge);
            }

            math::vec3 localNormal = math::inverse(transform) * math::vec4(comparisonNormal,0);
            polygon = std::make_shared<SplittablePolygon>(edgesInPolygon, localNormal);
            
            polygon->AssignEdgeOwnership();
            polygon->IdentifyBoundaries(transform);

            return true;
        }

        void SplitMesh(math::vec3 position, math::vec3 normal, bool keepBelow = true)
        {
            debugHelper.cuttingSetting.first = position;
            debugHelper.cuttingSetting.second = normal;

            auto [posH, rotH, scaleH] = owner.get_component_handles<transform>();
            const math::mat4& transform = math::compose(scaleH.read(), rotH.read(), posH.read());

            //for each polygon in polygonMesh
                //check split state of the polygon
            for (auto polygon : meshPolygons)
            {
                polygon->CalculatePolygonSplit(transform, position, normal);
            }

            SplitState requestedState = keepBelow ? SplitState::Below : SplitState::Above;

            //find first intersecting or requested polygon
            SplittablePolygonPtr initialFound = nullptr;

            bool foundUnvisited =
                FindFirstIntersectingOrRequestedState
                (initialFound, requestedState);

            //while(cannot find unvisited or requested)
            while (foundUnvisited)
            {
                std::vector<SplittablePolygonPtr> splitMesh;
                std::vector<SplittablePolygonPtr> nonSplitMesh;

                //------------------------ BFS search polygons that are in the same island -------------------------------------------//
                //------------------------ and put them divide them into a list of intersecting and nonsplit--------------------------//
                BFSFindRequestedAndIntersecting(
                    initialFound,
                    splitMesh,
                    nonSplitMesh,requestedState);

                //---------------------------------- Copy all polygons in splitMesh and nonSplitMesh ----------------------------------//
                CopyPolygons(splitMesh, nonSplitMesh);

                //find holes is newly created mesh
                std::vector < std::vector <meshHalfEdgePtr>> holeIslands;
                std::vector<std::vector<SplittablePolygonPtr>> intersectionIslands;

                //----------------------------------- Detect multiple holes in mesh  --------------------------------------------------//
                DetectIntersectionIsland(splitMesh, intersectionIslands);

                for ( std::vector<SplittablePolygonPtr>& intersectionIsland : intersectionIslands)
                {
                    for (SplittablePolygonPtr islandPolygon: intersectionIsland)
                    {
                        categorizeEdges(islandPolygon,transform,position,normal,requestedState);
                    }
                }

                //---------------------------------- Add intersecting and nonsplit to primitive mesh ------------------------------------------//
                //add stuff here

                std::vector< SplittablePolygonPtr> resultPolygon;

                resultPolygon.insert(resultPolygon.end()
                    , std::make_move_iterator(splitMesh.begin()), std::make_move_iterator(splitMesh.end()));

                resultPolygon.insert(resultPolygon.end(),
                    std::make_move_iterator(nonSplitMesh.begin()), std::make_move_iterator(nonSplitMesh.end()));
                
                PrimitiveMesh newMesh(owner, resultPolygon, ownerMaterialH);
                newMesh.InstantiateNewGameObject();

                initialFound->isVisited = true;

                foundUnvisited =
                    FindFirstIntersectingOrRequestedState
                    (initialFound, requestedState);
            }
        }

        void BFSFindRequestedAndIntersecting(
            SplittablePolygonPtr& intialPolygon,
            std::vector<SplittablePolygonPtr>& originalSplitMesh,
            std::vector<SplittablePolygonPtr>& originalNonSplitMesh,SplitState requestedState)
        {
            std::queue<SplittablePolygonPtr> unvisitedPolygonQueue;
            unvisitedPolygonQueue.push(intialPolygon);


            while (!unvisitedPolygonQueue.empty())
            {
               auto polygonPtr = unvisitedPolygonQueue.front();
               unvisitedPolygonQueue.pop();

               if (!polygonPtr->isVisited)
               {
                   polygonPtr->isVisited = true;

                   auto polygonSplitState = polygonPtr->GetPolygonSplitState();

                   bool polygonAtRequestedState = polygonSplitState == requestedState;
                   bool polygonAtIntersection = polygonSplitState == SplitState::Split;

                   if (polygonAtRequestedState)
                   {
                       originalNonSplitMesh.push_back(polygonPtr);
                       debugHelper.nonIntersectionPolygons.push_back(polygonPtr->localCentroid);
                   }
                   else if (polygonAtIntersection)
                   {
                       originalSplitMesh.push_back(polygonPtr);
                       debugHelper.intersectionsPolygons.push_back(polygonPtr->localCentroid);
                   }


                   if (polygonAtIntersection || polygonAtRequestedState)
                   {
                       for (auto edge : polygonPtr->GetMeshEdges())
                       {
                           bool isBoundary = edge->isBoundary;
                           bool hasPairing = (edge->pairingEdge) != nullptr;
                           bool pairingHasOwner = edge->pairingEdge->owner.lock() != nullptr;

                           if (isBoundary && hasPairing && pairingHasOwner)
                           {
                               unvisitedPolygonQueue.push(edge->pairingEdge->owner.lock());
                           }
                           
                       }
                   }


               }


            }
            //while unvisited polygon queue is not empty
                //pop polygon from queue

                //if not visited
                    //place polygon in correct list
                    //mark as visited

                    //get boundaries for neighbors and place in unvisited queue
                    


        }

        void CopyPolygons(std::vector<SplittablePolygonPtr>& originalSplitMesh,
            std::vector<SplittablePolygonPtr>& originalNonSplitMesh)
        {

        }

        void DetectIntersectionIsland(std::vector<SplittablePolygonPtr>& splitPolygons,
            std::vector<std::vector<SplittablePolygonPtr>>& intersectionIslands)
        {
            for (auto pol : splitPolygons) { pol->isVisited = false; }

            //find first intersection polygon
            SplittablePolygonPtr initialPolygon;
            bool foundUnvisited = FindFirstUnivistedIntersectionPolygon(splitPolygons, initialPolygon);


            //while can find intersection polygon
            while (foundUnvisited)
            {
                std::vector< math::vec3> DEBUG_ONLY_polygonPositions;
                std::vector< SplittablePolygonPtr> intersectionIsland;
                std::queue< SplittablePolygonPtr> unvisitedPolygons;
                unvisitedPolygons.push(initialPolygon);

                while (!unvisitedPolygons.empty())
                {
                    auto polygon = unvisitedPolygons.front();
                    unvisitedPolygons.pop();

                    if (!polygon->isVisited && polygon->GetPolygonSplitState() == SplitState::Split)
                    {
                        polygon->isVisited = true;
                        intersectionIsland.push_back(polygon);
                        DEBUG_ONLY_polygonPositions.push_back(polygon->localCentroid);

                        for (auto edge : polygon->GetMeshEdges())
                        {
                            auto pairingEdge = edge->pairingEdge;

                            if (edge->isBoundary && pairingEdge)
                            {
                                if (auto pairingPolygon = pairingEdge->owner.lock())
                                {
                                    unvisitedPolygons.push(pairingPolygon);
                                }
  
                            }
                        }
                    }
                }

                intersectionIslands.push_back(intersectionIsland);
                debugHelper.intersectionIslands.push_back(DEBUG_ONLY_polygonPositions);

                foundUnvisited = FindFirstUnivistedIntersectionPolygon(splitPolygons, initialPolygon);

            }

            

        }

        void categorizeEdges(SplittablePolygonPtr splitPolygon
            ,const math::mat4& transform,const math::vec3 cutPosition
            ,const math::vec3 cutNormal,SplitState requestedState)
        {
            BoundaryEdgeInfo polygonDebugInfo;
          
            bool keepAbove = requestedState == SplitState::Above ? true : false;
            //For now assume no islands in one polygon

            std::vector<meshHalfEdgePtr> unEffectedUsedEdges;
            std::vector<meshHalfEdgePtr> effectedUsedEdges;

            //[1] Get Triangles Effected by the split

            splitPolygon->ResetEdgeVisited();

            if (splitPolygon->GetMeshEdges().size() < 3) { return; }

            meshHalfEdgePtr initalEdge = splitPolygon->GetMeshEdges().at(0);
            //intialize unvisited queue
            std::queue<meshHalfEdgePtr> unvisitedEdges;
            unvisitedEdges.push(initalEdge);

            //BFS Find edges effected and uneffected by the split that are at the requested side of the split
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


                
            //[2] Get used boundary Effected

            for (auto edge : effectedUsedEdges)
            {
                edge->isVisited = false;
            }

            {
                std::vector<meshHalfEdgePtr> tempEdges{ std::move(effectedUsedEdges) };
                effectedUsedEdges.clear();

                //for each effectedUsedEdges
                for (auto edge : tempEdges)
                {
                    bool isAtLeastPartiallyAtRequestedSpot = keepAbove ?
                        edge->isEdgePartlyAbovePlane(transform, cutPosition, cutNormal)
                        : edge-> isEdgePartlyBelowPlane(transform, cutPosition, cutNormal);

                    bool isBoundaryEffectedOrPolygonBoundary = edge->isBoundary || edge->pairingEdge->isVisited;

                    if (isAtLeastPartiallyAtRequestedSpot && isBoundaryEffectedOrPolygonBoundary)
                    {
                        effectedUsedEdges.push_back(edge);
                    }

                }


            }

           
            
            //[4] Sort Edges

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

            math::vec3 polygonNormalCrossCutNormal = math::normalize(math::cross(worldPolygonNormal,cutNormal ));

            //SortingCriterium splittingPlaneSorter(worldCentroid,polygonNormalCrossCutNormal,transform);

            auto initialSorter = [&transform,&worldCentroid,&polygonNormalCrossCutNormal]
            ( const std::shared_ptr<MeshHalfEdge>& lhs, std::shared_ptr<MeshHalfEdge>& rhs)
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
           
            math::vec3 worldFirstEdge = splitEdges.at(0)->GetWorldCentroid(transform);
            math::vec3 worldSecondEdge = splitEdges.at(splitEdges.size() - 1)->GetWorldCentroid(transform);

            //use min max edges as new sorting direction

            //sort effectedUsedEdges based on sorting direction

            math::vec3 sortingCentroid = (worldFirstEdge + worldSecondEdge) / 2.0f;
            math::vec3 sortingDirection = worldSecondEdge - worldFirstEdge;

            auto boundarySorter = [&transform,&sortingCentroid,&sortingDirection]
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


            //[5] Regenerate Edges
            //check if current polygon point is above splitting plane

            

            //get start and end intersection points
            auto [firstEdgeCurrent, firstEdgeNext] = firstEdge->GetEdgeWorldPositions(transform);
            auto [secondEdgeCurrent, secondEdgeNext] = secondEdge->GetEdgeWorldPositions(transform);

            bool startFromOutsideIntersection =
                keepAbove == PhysicsStatics::IsPointAbovePlane(cutNormal, cutPosition, firstEdgeCurrent);

            //Find the intersection points of both edges toward the plane

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
                        generatedHalfEdges, i, supportEdge, vertexIntersectionInfo,polygonDebugInfo);
                }
                else
                {
                    OutsideIntersectionMeshRegeneration(
                        transform, effectedUsedEdges,
                        generatedHalfEdges, i, supportEdge, vertexIntersectionInfo,polygonDebugInfo);
                }


            }


            polygonDebugInfo.intersectionPoints.first = firstEdgeIntersection;
            polygonDebugInfo.intersectionPoints.second = secondEdgeIntersection;

            polygonDebugInfo.finalSortingDirection.first = sortingCentroid;
            polygonDebugInfo.finalSortingDirection.second = sortingDirection;

            polygonDebugInfo.drawColor = splitPolygon->debugColor;

            polygonDebugInfo.boundaryEdges.insert(
                polygonDebugInfo.boundaryEdges.begin(),
                effectedUsedEdges.begin(),
                effectedUsedEdges.end());

            polygonDebugInfo.boundaryEdges.insert(
                polygonDebugInfo.boundaryEdges.end(),
                generatedHalfEdges.begin(),
                generatedHalfEdges.end());

            debugHelper.boundaryEdgesForPolygon.push_back(polygonDebugInfo);

            auto&meshEdges = splitPolygon->GetMeshEdges();

            meshEdges.clear();

            meshEdges.insert(meshEdges.end(), unEffectedUsedEdges.begin(), unEffectedUsedEdges.end());
            meshEdges.insert(meshEdges.end(), effectedUsedEdges.begin(), effectedUsedEdges.end());
            meshEdges.insert(meshEdges.end(), generatedHalfEdges.begin(), generatedHalfEdges.end());



            debugHelper.polygonCount++;
        }

        bool FindFirstIntersectingOrRequestedState(SplittablePolygonPtr& outfirstFound, SplitState requestedState)
        {
            for (auto polygon : meshPolygons)
            {
                if (!polygon->isVisited)
                {
                    auto polygonSplitState = polygon->GetPolygonSplitState();

                    if (polygonSplitState == SplitState::Split
                        || polygonSplitState == requestedState)
                    {
                        outfirstFound = polygon;
                        return true;
                    }
                }
            }

            return false;
        }

        bool FindFirstUnivistedIntersectionPolygon(std::vector< SplittablePolygonPtr>& splitPolygon, SplittablePolygonPtr& foundPolygon)
        {
            for (auto polygonPtr : splitPolygon)
            {
                if (!polygonPtr->isVisited)
                {
                    foundPolygon = polygonPtr;
                    return true;
                }
            }

            return false;
        }

        //------------------------------------------------------------------- Mesh Regeneration Stuff----------------------------------------------------------//

        void InsideIntersectionMeshRegeneration(const math::mat4& transform, std::vector<meshHalfEdgePtr>& effectedBoundaryEdges,
            std::vector<meshHalfEdgePtr>& generatedEdges, int i
            , meshHalfEdgePtr& supportEdge,
            const SplitterIntersectionInfo& vertexIntersectionInfo, BoundaryEdgeInfo& debugStuff)
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
                    (baseEdge->nextEdge->position,  baseEdge->nextEdge->uv);

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

                nextSupportEdge->position = math::inverse(transform) * math::vec4((worldPosition),1);
                nextSupportEdge->uv = initialUVIntersection + startToEndUV;
            }
            else
            {
                math::vec3 worldPosition = initialIntersectionPosition
                    + startToEndPosition * (float)i / maxData;

                math::vec2 uv = initialUVIntersection + startToEndUV * (float)i / maxData;

                nextSupportEdge = std::make_shared< MeshHalfEdge>(
                    math::inverse(transform) * math::vec4(worldPosition,1), uv);

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
                * math::vec4(worldIntersectionPosition,1);

            math::vec2 uv = initialUVIntersection + startToEndUV * interpolant;

            meshHalfEdgePtr intersectionEdge = std::make_shared< MeshHalfEdge>(
                localIntersectionPosition, uv);


            generatedEdges.push_back(intersectionEdge);
            intersectionEdge->isBoundary = true;

            debugStuff.intersectionEdge = intersectionEdge->GetEdgeWorldPosition(transform);

            CreateNonAllignedQuad(currentSupportEdge, nextSupportEdge, baseEdge, intersectionEdge, generatedEdges);

            supportEdge = nextSupportEdge;



        }

        void CreateNonAllignedQuad(
            meshHalfEdgePtr currentSupport, meshHalfEdgePtr nextSupport, meshHalfEdgePtr baseEdge, meshHalfEdgePtr intersectionEdge,
            std::vector<meshHalfEdgePtr>& generatedEdges
        )
        {
            //create new supporttriangle located at next support
            meshHalfEdgePtr supportTriangle = std::make_shared< MeshHalfEdge>(nextSupport->position,nextSupport->uv);


            //currentSupport-intersection-supporttriangle
            MeshHalfEdge::ConnectIntoTriangle(currentSupport, intersectionEdge, supportTriangle);

            //create new nextsupporttriangle located at currentsupport
            meshHalfEdgePtr nextSupportTriangle = std::make_shared< MeshHalfEdge>(currentSupport->position,currentSupport->uv);

            //nextsupporttriangle-nextSupport-baseEdge
            MeshHalfEdge::ConnectIntoTriangle(nextSupportTriangle, nextSupport, baseEdge);

            supportTriangle->SetPairing(nextSupportTriangle);
            generatedEdges.push_back(supportTriangle);
            generatedEdges.push_back(nextSupportTriangle);

            
        }

        void OutsideIntersectionMeshRegeneration(const math::mat4& transform, std::vector<meshHalfEdgePtr>& effectedBoundaryEdges,
            std::vector<meshHalfEdgePtr>& generatedEdges, int i
            , meshHalfEdgePtr supportEdge,
            const SplitterIntersectionInfo& vertexIntersectionInfo, BoundaryEdgeInfo& debugStuff)
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
                    math::vec4(initialIntersectionPosition,1);

                currentSupportEdge->uv = initialUVIntersection;

            }
            else
            {
                currentSupportEdge = std::make_shared<MeshHalfEdge>
                    (supportEdge->nextEdge->position, supportEdge->nextEdge->uv);

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
                nextSupportEdge = std::make_shared<MeshHalfEdge>(baseEdge->nextEdge->position, baseEdge->nextEdge->uv);
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
                (localIntersectionEdgePosition, edgeUV);

            intersectionEdge->isBoundary = true;

            generatedEdges.push_back(intersectionEdge);


            CreateAllignedQuad(currentSupportEdge, nextSupportEdge, baseEdge,
                intersectionEdge, generatedEdges);

        }

        void CreateAllignedQuad(meshHalfEdgePtr currentSupport, meshHalfEdgePtr nextSupport, meshHalfEdgePtr baseEdge,
            meshHalfEdgePtr intersectionEdge, std::vector<meshHalfEdgePtr>& generatedEdges)
        {
            //create new supporttriangle located at next support
            meshHalfEdgePtr supportTriangle = std::make_shared<MeshHalfEdge>(nextSupport->position,nextSupport->uv);


            //currentSupport-intersection-supporttriangle
            MeshHalfEdge::ConnectIntoTriangle(currentSupport, supportTriangle, intersectionEdge);

            //create new nextsupporttriangle located at currentsupport
            meshHalfEdgePtr  nextSupportTriangle = std::make_shared<MeshHalfEdge>(currentSupport->position,currentSupport->uv);

            //nextsupporttriangle-nextSupport-baseEdge
            MeshHalfEdge::ConnectIntoTriangle(nextSupportTriangle, baseEdge, nextSupport);

            supportTriangle->SetPairing(nextSupportTriangle);

            generatedEdges.push_back(supportTriangle);
            generatedEdges.push_back(nextSupportTriangle);
        }




    };
}



