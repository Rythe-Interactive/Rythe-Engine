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
#include <physics/mesh_splitter_utils/intersecting_polygon_organizer.hpp>
#include <physics/mesh_splitter_utils/mesh_split_params.hpp>
#include <physics/mesh_splitter_utils/intersection_edge_info.h>

namespace legion::physics
{
    struct MeshSplitter
    {

        ecs::entity_handle owner;
        std::vector<ecs::entity_handle> splitTester;

        rendering::material_handle ownerMaterialH;

        std::vector<SplittablePolygonPtr> meshPolygons;

        MeshSplitterDebugHelper debugHelper;

        void TestSplit()
        {

            if (!splitTester.empty())
            {
                std::vector<MeshSplitParams> splittingPlanes;

                for (auto splitObject : splitTester)
                {
                    auto [posH, rotH, scaleH] = splitObject.get_component_handles<transform>();
                    const math::mat4 transform = math::compose(scaleH.read(), rotH.read(), posH.read());
                    const math::vec3 worldUp = transform * math::vec4(0, 1, 0, 0);

                    splittingPlanes.push_back(MeshSplitParams(posH.read(),math::normalize(worldUp)));
                  
                }
                
                MultipleSplitMesh(splittingPlanes);

            }
            else
            {
                log::error("Split tester not set");
            }
        }

        void DestroyTestSplitter(ecs::EcsRegistry* m_ecs)
        {
            for (auto splitObject : splitTester)
            {
                m_ecs->destroyEntity(splitObject);
            }

            splitTester.clear();
           
        }

        /** @brief Creates a Half-Edge Data structure around the mesh and
        * @param entity the entity that this MeshSplitter is attached to
        */
        void InitializePolygons(ecs::entity_handle entity)
        {
            owner = entity;

            auto [meshFilter, meshRenderer] = entity.get_component_handles<rendering::renderable>();

            ownerMaterialH = meshRenderer.read().material;

            auto [posH, rotH, scaleH] = entity.get_component_handles<transform>();

            if (meshFilter && posH && rotH && scaleH)
            {
                log::debug("Mesh and Transform found");
                std::queue<meshHalfEdgePtr> meshHalfEdges;

                //auto renderable = renderable.read();
                mesh& mesh = meshFilter.read().get().second;

                const math::mat4 transform = math::compose(scaleH.read(), rotH.read(), posH.read());

                HalfEdgeFinder edgeFinder;
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

            math::vec3 localNormal = math::inverse(transform) * math::vec4(comparisonNormal, 0);
            polygon = std::make_shared<SplittablePolygon>(edgesInPolygon, localNormal);

            polygon->AssignEdgeOwnership();
            polygon->IdentifyBoundaries(transform);

            return true;
        }

        void SplitPolygons
        (   std::vector<SplittablePolygonPtr>& polygonsToSplit,
            const math::vec3& planeNormal,
            const math::vec3& planePosition,
            const math::mat4& transform,
            std::vector<std::vector<SplittablePolygonPtr>>& resultingIslands,
            bool keepBelow = true)
        {
            log::debug("SplitPolygons");
            for (auto polygon : polygonsToSplit)
            {
                polygon->isVisited = false;
                polygon->CalculatePolygonSplit(transform, planePosition, planeNormal);
                /*log::debug("-------------");
                log::debug(" polygon->visited {} " ,polygon->isVisited);
                log::debug(" polygon edge count {} ", polygon->GetMeshEdges().size());
                switch (polygon->GetPolygonSplitState())
                {
                case SplitState::Above:
                    log::debug("Above");
                    break;
                case SplitState::Split:
                    log::debug("Split");
                    break;
                case SplitState::Below:
                    log::debug("Below");
                    break;
                default:
                    break;

               

                }
                int bCount = 0;

                for (auto edge : polygon->GetMeshEdges())
                {
                    if (edge->isBoundary)
                    {
                        bCount++;
                    }
                }
                log::debug(" polygon boundary count {} ", bCount);
                log::debug("-------------");*/
            }

            SplitState requestedState = keepBelow ? SplitState::Below : SplitState::Above;

            SplittablePolygonPtr initialFound = nullptr;

            bool foundUnvisited =
                FindFirstIntersectingOrRequestedState
                (initialFound, requestedState,polygonsToSplit);

            log::debug("-> Start while loop");

            for (auto polygon : polygonsToSplit)
            {
                log::debug("-> checking polygon ");
                //log::debug("polygon is intersecting ");

                //polygon->AssignEdgeOwnership();

                //if (polygon->isIntersectingPart)
                //{
                //    DebugBreak();
                //    for (auto edge : polygon->GetMeshEdges())
                //    {
                //        if (edge->isBoundary)
                //        {
                //            auto pairing = edge->pairingEdge;

                //            auto pairingpairing = pairing->pairingEdge;

                //           /* std::string pairingID = pairing->debugID;
                //            std::string pairingpairingID = pairingpairing->debugID;*/

                //            assert(pairingpairing == edge);

                //        }
                //    }
                //}
            }

            while (foundUnvisited)
            {
                std::vector<SplittablePolygonPtr> splitMesh;
                std::vector<SplittablePolygonPtr> nonSplitMesh;

               

                //------------------------ BFS search polygons that are in the same island -------------------------------------------//
                //------------------------ and  divide them into a list of split and nonsplit polygons--------------------------//
                initialFound->isVisited = false;
                BFSFindRequestedAndIntersecting(
                    initialFound,
                    splitMesh,
                    nonSplitMesh, requestedState);

       /*         log::debug("split containes");
                for (auto pol : splitMesh)
                {
                    log::debug("pol is intersecting? {} " , pol->isIntersectingPart);
                }*/
                    

                //find holes is newly created mesh
                std::vector < std::vector <meshHalfEdgePtr>> holeIslands;
                std::vector<std::vector<SplittablePolygonPtr>> intersectionIslands;

                //----------------------------------- Detect multiple holes in mesh  --------------------------------------------------//

                DetectIntersectionIsland(splitMesh, intersectionIslands);

                std::vector<IntersectionEdgeInfo> generatedIntersectionEdges;

                //----------------------------------- Filter Edges in polygon in order to fit sliced mesh --------------------------------------------------//

                for (std::vector<SplittablePolygonPtr>& intersectionIsland : intersectionIslands)
                {
                    for (SplittablePolygonPtr islandPolygon : intersectionIsland)
                    {
                        categorizeEdges(islandPolygon, transform, planePosition, planeNormal, requestedState, generatedIntersectionEdges);
                    }
                }

                math::vec3 localNormal = transform * math::vec4(planeNormal, 0);
                SplittablePolygonPtr intersectionPolygon = CreateIntersectionPolygon(generatedIntersectionEdges, math::normalize(localNormal));
                intersectionPolygon->isVisited = true;
                intersectionPolygon->ResetEdgeVisited();
                //intersectionPolygon->isIntersectingPart = true;
                //---------------------------------- Add intersecting and nonsplit to primitive mesh ---------------------------------//
                

                std::vector< SplittablePolygonPtr> resultPolygons;

                resultPolygons.insert(resultPolygons.end()
                    , std::make_move_iterator(splitMesh.begin()), std::make_move_iterator(splitMesh.end()));

                resultPolygons.insert(resultPolygons.end()
                    , std::make_move_iterator(nonSplitMesh.begin()), std::make_move_iterator(nonSplitMesh.end()));

                resultPolygons.push_back(std::move(intersectionPolygon));

                

                foundUnvisited =
                    FindFirstIntersectingOrRequestedState
                    (initialFound, requestedState,polygonsToSplit);

                resultingIslands.push_back(std::move(resultPolygons));

            }


        }



        void MultipleSplitMesh(const std::vector<MeshSplitParams>& splittingPlanes, bool keepBelow = true)
        {
            auto [posH, rotH, scaleH] = owner.get_component_handles<transform>();
            const math::mat4& transform = math::compose(scaleH.read(), rotH.read(), posH.read());

            //copy original mesh

            std::vector< std::vector<SplittablePolygonPtr>> outputPolygonIslandsGenerated;
            outputPolygonIslandsGenerated.push_back(std::move(meshPolygons));
            meshPolygons.clear();

            //for each splitting plane in splittingPlanes
            for (const MeshSplitParams& splitParam : splittingPlanes)
            {
                std::vector< std::vector<SplittablePolygonPtr>> inputList = std::move(outputPolygonIslandsGenerated);
                outputPolygonIslandsGenerated.clear();

                for (std::vector<SplittablePolygonPtr>& polygonIsland : inputList)
                {
                    for (SplittablePolygonPtr polygon : polygonIsland)
                    {
                        polygon->isVisited = false;
                    }

                    SplitPolygons
                    (polygonIsland,
                        splitParam.planeNormal,
                        splitParam.planePostion,
                        transform,
                        outputPolygonIslandsGenerated);
                }
            }

            for (auto& polygonIsland : outputPolygonIslandsGenerated)
            {
                PrimitiveMesh newMesh(owner, polygonIsland, ownerMaterialH);
                newMesh.InstantiateNewGameObject();
            }


        }

        void SplitMesh(math::vec3 position, math::vec3 normal, bool keepBelow = true)
        {
            debugHelper.cuttingSetting.first = position;
            debugHelper.cuttingSetting.second = normal;

            auto [posH, rotH, scaleH] = owner.get_component_handles<transform>();
            const math::mat4& transform = math::compose(scaleH.read(), rotH.read(), posH.read());

            std::vector<std::vector<SplittablePolygonPtr>> resultingIsland;
            SplitPolygons(meshPolygons, normal, position, transform,resultingIsland, keepBelow);

            for (auto& polygonIsland : resultingIsland)
            {
                PrimitiveMesh newMesh(owner, polygonIsland, ownerMaterialH);
                newMesh.InstantiateNewGameObject();
            }
        }

        void BFSFindRequestedAndIntersecting(
            SplittablePolygonPtr& intialPolygon,
            std::vector<SplittablePolygonPtr>& originalSplitMesh,
            std::vector<SplittablePolygonPtr>& originalNonSplitMesh, SplitState requestedState)
        {
            //DebugBreak();
            std::queue<SplittablePolygonPtr> unvisitedPolygonQueue;
            unvisitedPolygonQueue.push(intialPolygon);


            while (!unvisitedPolygonQueue.empty())
            {
                auto polygonPtr = unvisitedPolygonQueue.front();
                unvisitedPolygonQueue.pop();

                //if (polygonPtr->isIntersectingPart)
                //{
                //    DebugBreak();
                //}

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
                        
                            if (isBoundary && hasPairing)
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

        std::vector<SplittablePolygonPtr>& CopyPolygons(std::vector<SplittablePolygonPtr>& originalSplitMesh)
        {
            std::vector<SplittablePolygonPtr> result;

            //copy each edge in mesh
            for (SplittablePolygonPtr polygon : originalSplitMesh)
            {
                //copy polygon and place on shadow edge

            }

                



            return result;
        }

        void CopyEdgeVector(std::vector<meshHalfEdgePtr>& originalHalfEdgeList, std::vector<meshHalfEdgePtr>& resultCopyList)
        {
            for (meshHalfEdgePtr originalEdge : originalHalfEdgeList)
            {
                //copy polygon and place on shadow edge

            }

            



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
            , const math::mat4& transform, const math::vec3 cutPosition
            , const math::vec3 cutNormal, SplitState requestedState, std::vector<IntersectionEdgeInfo>& generatedIntersectionEdges)
        {
            IntersectingPolygonOrganizer polygonOrganizer(&debugHelper);
            polygonOrganizer.categorizeEdges(splitPolygon, transform, cutPosition, cutNormal, requestedState,generatedIntersectionEdges);
        }

        bool FindFirstIntersectingOrRequestedState(SplittablePolygonPtr& outfirstFound, SplitState requestedState
            , std::vector<SplittablePolygonPtr>& polygonList)
        {
            for (auto polygon : polygonList)
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

        SplittablePolygonPtr CreateIntersectionPolygon(
            std::vector<IntersectionEdgeInfo>& generatedIntersectionEdges,
            const math::vec3& localSplitNormal)
        {
            //log::debug("CreateIntersectionPolygon");

            std::vector<meshHalfEdgePtr> edgesCreated;

            math::vec3 localCentroid{};

            //---------------------------------- Instantiate Edges and connect them into a triangle -------------------------------------//

            for (IntersectionEdgeInfo& info : generatedIntersectionEdges)
            {
                //instantiate edge and set its pairing
                meshHalfEdgePtr firstEdge = std::make_shared<MeshHalfEdge>(info.first);
                meshHalfEdgePtr secondEdge = std::make_shared<MeshHalfEdge>(info.second);
                //temporarily second edge to info.second
                meshHalfEdgePtr thirdEdge = std::make_shared<MeshHalfEdge>(info.second);

                //info.pairingToConnectTo->debugID = "ShouldConnecTo";
                

                info.centroidEdge = thirdEdge;
                info.instantiatedEdge  = firstEdge;

                info.pairingToConnectTo->SetPairing(info.instantiatedEdge);
                
                info.instantiatedEdge->isBoundary = true;

                MeshHalfEdge::ConnectIntoTriangle(firstEdge, secondEdge, thirdEdge);
            
                info.instantiatedEdge->populateVectorWithTriangle(edgesCreated);

                //info.instantiatedEdge->debugID = "InstantiatedEdge";

                /*assert(firstEdge != info.pairingToConnectTo);

                assert(firstEdge->pairingEdge == info.pairingToConnectTo);

                assert(info.pairingToConnectTo->pairingEdge == firstEdge);

                assert(info.pairingToConnectTo->pairingEdge != info.pairingToConnectTo);*/
                localCentroid += info.first;
            }

            localCentroid /= (float)generatedIntersectionEdges.size();

            //---------------------------------- Set the centroid edge to the local centroid of the polygon  -------------------------------------//

            for (IntersectionEdgeInfo& info : generatedIntersectionEdges)
            {
                info.centroidEdge->position = localCentroid;
                assert(info.instantiatedEdge->nextEdge->nextEdge);
                
            }

            //---------------------------------- Set pairing information of all edges ---------------------------------------------//

            for (IntersectionEdgeInfo& info : generatedIntersectionEdges)
            {
                float currentClosestDistance = std::numeric_limits<float>::max();
                meshHalfEdgePtr closestEdge = nullptr;
                //get closest unvisisted IntersectionEdgeInfo

                const math::vec3& pointToCompare = info.second;
                //log::debug("Finding distance");
                //find edge closest to pointToCompare
                for (IntersectionEdgeInfo& otherInfo : generatedIntersectionEdges)
                {
            
                    assert(otherInfo.instantiatedEdge);
                    if (info.instantiatedEdge == otherInfo.instantiatedEdge) { continue; }

                    float distanceFound = math::distance2(pointToCompare, otherInfo.first);
                    //log::debug("distanceFound {}" , distanceFound);
                    if (distanceFound < currentClosestDistance)
                    {
                        currentClosestDistance = distanceFound;
                        closestEdge = otherInfo.centroidEdge;
                    }

                }

                assert(closestEdge);
                auto infoEdge = info.instantiatedEdge->nextEdge;
                auto infoEdgePairing = closestEdge;

                infoEdge->SetPairing(infoEdgePairing);

            }

            auto polygon = std::make_shared<SplittablePolygon>(edgesCreated, localSplitNormal);
            polygon->AssignEdgeOwnership();

            return  polygon;
        };
    };

}

