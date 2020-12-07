#pragma once

#include <core/core.hpp>
#include <physics/mesh_splitter_utils/mesh_splitter_typedefs.hpp>
#include <physics/mesh_splitter_utils/mesh_half_edge.hpp>
#include <physics/mesh_splitter_utils/splittable_polygon.h>
#include <rendering/components/renderable.hpp>
#include <physics/mesh_splitter_utils/primitive_mesh.h>
#include <rendering/components/renderable.hpp>

namespace legion::physics
{
    struct BoundaryEdgeInfo
    {
        std::vector<meshHalfEdgePtr> boundaryEdges;
        math::color drawColor;
    };

    struct MeshSplitterDebugHelper
    {
        std::vector<math::vec3> intersectionsPolygons;
        std::vector<math::vec3> nonIntersectionPolygons;
        std::vector< std::vector<math::vec3>> intersectionIslands;

        std::vector<BoundaryEdgeInfo> boundaryEdgesForPolygon;

        std::array< math::color,5> colors =
        {   math::colors::red,
            math::colors::cyan,
            math::colors::green,
            math::colors::magenta,
            math::colors::yellow
        };

        int polygonCount;

        int polygonToDisplay = 0;
        
    };

    struct MeshSplitter
    {
        ecs::entity_handle owner;
        ecs::entity_handle splitTester;

        rendering::material_handle ownerMaterialH;
        

        meshHalfEdgePtr currentPtr;

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

                FindHalfEdge(mesh, transform, meshHalfEdges);

                BFSPolygonize(meshHalfEdges, transform);

                log::debug("Mesh vertices {}, Mesh indices {}", mesh.vertices.size(), mesh.indices.size());


            }
            else
            {
                log::warn("The given entity does not have a meshHandle!");
            }

        }

        /** @brief Creates a Half-Edge Data structure around the given mesh
        * and inserts them into the given queue
        */
        void FindHalfEdge
        (mesh& mesh, const math::mat4& transform,
            std::queue<meshHalfEdgePtr>& meshHalfEdges)
        {
            VertexIndexToHalfEdgePtr indexToEdgeMap;

            //[1] find the unique vertices of a mesh. We are currently keeping a list of found
            //vertices and comparing them to our current vertices

            // holds the "pointer" to the unique indices inside mesh->Indices
            std::vector<int> uniqueIndex;

            // stores the unique vertices of the mesh
            std::vector<math::vec3> uniquePositions;

            int uniqueIndexCount = -1;

            auto& vertices = mesh.vertices;
            auto& indices = mesh.indices;

            for (int i = 0; i < vertices.size(); ++i)
            {
                math::vec3 position = vertices.at(i);

                bool isVectorSeen = false;

                //have we found this vector before?
                for (int j = 0; j < uniquePositions.size(); j++)
                {
                    math::vec3 transformedPos = transform * math::vec4(position, 1);
                    math::vec3 uniqueTransformedPos = transform * math::vec4(uniquePositions[j], 1);

                    float dist = math::distance(uniqueTransformedPos, transformedPos);

                    if (math::epsilonEqual<float>(dist, 0.0f, math::epsilon<float>()))
                    {
                        //we have seen this vector before
                        uniqueIndex.push_back(j);
                        isVectorSeen = true;
                        break;
                    }
                }

                if (!isVectorSeen)
                {
                    //we have not seen this position before,add it 
                    uniqueIndexCount++;
                    uniqueIndex.push_back(uniqueIndexCount);

                    uniquePositions.push_back(position);

                }
            }


            //[2] use the unique vertices of a mesh to generate half-edge data structure

            for (int i = 0; i < indices.size(); i += 3)
            {
                int firstVertIndex = indices.at(i);
                int secondVertIndex = indices.at(i + 1);
                int thirdVertIndex = indices.at(i + 2);

                int uniqueFirstIndex = uniqueIndex.at(firstVertIndex);
                int uniqueSecondIndex = uniqueIndex.at(secondVertIndex);
                int uniqueThirdIndex = uniqueIndex.at(thirdVertIndex);

                //-----------------instantiate first half edge---------------------//
                //MeshHalfEdge firstEdge = std::make
                auto firstEdge = InstantiateEdge(firstVertIndex
                    , edgeVertexIndexPair(uniqueFirstIndex, uniqueSecondIndex), vertices, meshHalfEdges, indexToEdgeMap);

                //-----------------instantiate second half edge---------------------//
                auto secondEdge = InstantiateEdge(secondVertIndex
                    , edgeVertexIndexPair(uniqueSecondIndex, uniqueThirdIndex), vertices, meshHalfEdges, indexToEdgeMap);

                //-----------------instantiate third half edge---------------------//
                auto thirdEdge = InstantiateEdge(thirdVertIndex
                    , edgeVertexIndexPair(uniqueThirdIndex, uniqueFirstIndex), vertices, meshHalfEdges, indexToEdgeMap);


                firstEdge->nextEdge = secondEdge;
                secondEdge->nextEdge = thirdEdge;
                thirdEdge->nextEdge = firstEdge;


            }

            //[3] connect each edge with its pair based on index
            log::debug("Created {} edges ", meshHalfEdges.size());

            for (auto indexEdgePair : indexToEdgeMap)
            {

                int u = indexEdgePair.first.first;
                int v = indexEdgePair.first.second;

                //for a Halfedge paired with vertices with an index of (u,v),
                //its pair would be a HalfEdge paired with vertices with an index of (v,u)
                auto iter = indexToEdgeMap.find(edgeVertexIndexPair(v, u));

                if (iter != indexToEdgeMap.end())
                {

                    auto  edgePair = iter->second;
                    auto  otherEdge = indexEdgePair.second;

                    edgePair->pairingEdge = otherEdge;
                    otherEdge->pairingEdge = edgePair;

                }

            }


            currentPtr = meshHalfEdges.front();

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
                
                PrimitiveMesh newMesh(resultPolygon, ownerMaterialH);
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
            bool keepAbove = requestedState == SplitState::Above ? true : false;
            //For now assume no islands in one polygon

            std::vector<meshHalfEdgePtr> unEffectedUsedEdges;
            std::vector<meshHalfEdgePtr> effectedUsedEdges;

            //[1] Get Triangles Effected by the split


            

            for (auto edge : splitPolygon->GetMeshEdges())
            {
                edge->isVisited = false;
            }

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


                
            //[2] Get UsedBoundary Effected

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

            bool startFromOutsideIntersection =
                keepAbove == PhysicsStatics::IsPointAbovePlane(cutNormal, cutPosition, worldFirstEdge);

            //get start and end intersection points
            auto [firstEdgeCurrent, firstEdgeNext] = firstEdge->GetEdgeWorldPositions(transform);
            auto [secondEdgeCurrent, secondEdgeNext] = secondEdge->GetEdgeWorldPositions(transform);

            math::vec3 firstEdgeIntersection;
            PhysicsStatics::FindLineToPlaneIntersectionPoint(cutNormal, cutPosition,
                firstEdgeCurrent, firstEdgeNext, firstEdgeIntersection);

            math::vec3 secondEdgeIntersection;
            PhysicsStatics::FindLineToPlaneIntersectionPoint(cutNormal, cutPosition,
                secondEdgeCurrent, secondEdgeNext, secondEdgeIntersection);

            math::vec3 startToEndIntersection = secondEdgeIntersection - firstEdgeIntersection;

            meshHalfEdgePtr supportEdge = nullptr;
            //generated half edges using
            std::vector<meshHalfEdgePtr> generatedHalfEdges;

            for (int i = 1; i < effectedUsedEdges.size() - 1; i++)
            {
                if (startFromOutsideIntersection)
                {
                    InsideIntersectionMeshRegeneration(
                        transform, effectedUsedEdges,
                        generatedHalfEdges, i, supportEdge, firstEdgeIntersection, startToEndIntersection);
                }
                else
                {
                    OutsideIntersectionMeshRegeneration(
                        transform, effectedUsedEdges,
                        generatedHalfEdges, i, supportEdge, firstEdgeIntersection, startToEndIntersection);
                }


            }


            BoundaryEdgeInfo polygonDebugInfo;

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

        meshHalfEdgePtr InstantiateEdge(int vertexIndex
            , const std::pair<int, int> uniqueIndexPair
            , const std::vector<math::vec3>& vertices
            , std::queue<meshHalfEdgePtr>& edgePtrs
            , VertexIndexToHalfEdgePtr& indexToEdgeMap)
        {
            auto firstEdge = std::make_shared<MeshHalfEdge>(vertices[vertexIndex]);

            auto edgeToAdd = UniqueAdd(firstEdge, indexToEdgeMap, uniqueIndexPair);

            edgePtrs.push(edgeToAdd);

            return edgeToAdd;
        }

        meshHalfEdgePtr UniqueAdd(meshHalfEdgePtr newEdge, VertexIndexToHalfEdgePtr& indexToEdgeMap, edgeVertexIndexPair uniqueIndexPair)
        {
            auto iter = indexToEdgeMap.find(uniqueIndexPair);

            if (iter != indexToEdgeMap.end())
            {
                return iter->second;
            }

            auto pair = uniqueIndexPair;
            indexToEdgeMap.insert({ pair ,newEdge });

            //vertexIndexToHalfEdge.Add(key, value);
            return newEdge;
        }



        //------------------------------------------------------------------- Mesh Regeneration Stuff----------------------------------------------------------//

        void InsideIntersectionMeshRegeneration(const math::mat4& transform, std::vector<meshHalfEdgePtr>& effectedBoundaryEdges,
            std::vector<meshHalfEdgePtr>& generatedEdges, int i
            , meshHalfEdgePtr& supportEdge,
            math::vec3 worldStartIntersection,
            math::vec3 startToEndIntersection)
        {
            //select Base Half Edge
            meshHalfEdgePtr baseEdge = effectedBoundaryEdges[i];


            //----------------------------------- Create currentSupportEdge -------------------------------------------------------------//
            meshHalfEdgePtr currentSupportEdge = nullptr;

            if (i == 1)
            {
                currentSupportEdge = effectedBoundaryEdges[0];
                //currentSupportEdge.position = transform.worldToLocalMatrix.MultiplyPoint(worldStartIntersection);
            }
            else
            {
                currentSupportEdge = std::make_shared<MeshHalfEdge>(baseEdge->nextEdge->position);
                currentSupportEdge->SetPairing(supportEdge);
                generatedEdges.push_back(currentSupportEdge);

            }
            //----------------------------------- Create nextcurrentSupportEdge -------------------------------------------------------------//
            int maxData = effectedBoundaryEdges.size() - 1;
            meshHalfEdgePtr nextSupportEdge;

            if (i + 1 == effectedBoundaryEdges.size() - 1)
            {
                nextSupportEdge = effectedBoundaryEdges[effectedBoundaryEdges.size() - 1];

                math::vec3 worldPosition = (worldStartIntersection + startToEndIntersection);

                nextSupportEdge->position = math::inverse(transform) * math::vec4((worldPosition),1);
            }
            else
            {
                math::vec3 worldPosition =worldStartIntersection + startToEndIntersection * (float)i / maxData;
                nextSupportEdge = std::make_shared< MeshHalfEdge>(
                    math::inverse(transform) * math::vec4(worldPosition,1));

                generatedEdges.push_back(nextSupportEdge);
            }

            //----------------------------------- Create Intersection -------------------------------------------------------------//

            int currentIndex = i - 1;


            //i - 1 < 0 ? 0 : i - 1 ;
            float interpolant = (float)currentIndex / maxData;

            meshHalfEdgePtr intersectionEdge = std::make_shared< MeshHalfEdge>(
                math::inverse(transform) *  math::vec4(worldStartIntersection + (startToEndIntersection * interpolant),1));
            generatedEdges.push_back(intersectionEdge);


            CreateNonAllignedQuad(currentSupportEdge, nextSupportEdge, baseEdge, intersectionEdge, generatedEdges);

            supportEdge = nextSupportEdge;

        }

        void CreateNonAllignedQuad(
            meshHalfEdgePtr currentSupport, meshHalfEdgePtr nextSupport, meshHalfEdgePtr baseEdge, meshHalfEdgePtr intersectionEdge,
            std::vector<meshHalfEdgePtr>& generatedEdges
        )
        {
            //create new supporttriangle located at next support
            meshHalfEdgePtr supportTriangle = std::make_shared< MeshHalfEdge>(nextSupport->position);


            //currentSupport-intersection-supporttriangle
            MeshHalfEdge::ConnectIntoTriangle(currentSupport, intersectionEdge, supportTriangle);

            //create new nextsupporttriangle located at currentsupport
            meshHalfEdgePtr nextSupportTriangle = std::make_shared< MeshHalfEdge>(currentSupport->position);

            //nextsupporttriangle-nextSupport-baseEdge
            MeshHalfEdge::ConnectIntoTriangle(nextSupportTriangle, nextSupport, baseEdge);

            supportTriangle->SetPairing(nextSupportTriangle);
            generatedEdges.push_back(supportTriangle);
            generatedEdges.push_back(nextSupportTriangle);

            
        }

        void OutsideIntersectionMeshRegeneration(const math::mat4& transform, std::vector<meshHalfEdgePtr>& effectedBoundaryEdges,
            std::vector<meshHalfEdgePtr>& generatedEdges, int i
            , meshHalfEdgePtr supportEdge,
            math::vec3 worldStartIntersection,
            math::vec3 startToEndIntersection)
        {
            meshHalfEdgePtr baseEdge = effectedBoundaryEdges[i];

            //----------------------------------- Create currentSupportEdge -------------------------------------------------------------//
            meshHalfEdgePtr currentSupportEdge;
            if (i == 1)
            {
                currentSupportEdge = effectedBoundaryEdges[0];

                currentSupportEdge->position = math::inverse(transform) *
                    math::vec4( worldStartIntersection,1);

            }
            else
            {
                currentSupportEdge = std::make_shared<MeshHalfEdge>(supportEdge->nextEdge->position);

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
                nextSupportEdge = std::make_shared<MeshHalfEdge>(baseEdge->nextEdge->position);
                generatedEdges.push_back(nextSupportEdge);
            }

            supportEdge = nextSupportEdge;
            //----------------------------------- Create Intersection -------------------------------------------------------------//
            //Vector3 intersectionPosition;
            int maxData = effectedBoundaryEdges.size() - 1;
            int currentIndex = i + 1;
            float interpolant = (float)currentIndex / maxData;

            meshHalfEdgePtr intersectionEdge = std::make_shared<MeshHalfEdge>(
                math::inverse(transform) * math::vec4((worldStartIntersection + (startToEndIntersection * interpolant),1)));

            generatedEdges.push_back(intersectionEdge);


            CreateAllignedQuad(currentSupportEdge, nextSupportEdge, baseEdge,
                intersectionEdge, generatedEdges);

        }

        void CreateAllignedQuad(meshHalfEdgePtr currentSupport, meshHalfEdgePtr nextSupport, meshHalfEdgePtr baseEdge,
            meshHalfEdgePtr intersectionEdge, std::vector<meshHalfEdgePtr>& generatedEdges)
        {
            //create new supporttriangle located at next support
            meshHalfEdgePtr supportTriangle = std::make_shared<MeshHalfEdge>(nextSupport->position);


            //currentSupport-intersection-supporttriangle
            MeshHalfEdge::ConnectIntoTriangle(currentSupport, supportTriangle, intersectionEdge);

            //create new nextsupporttriangle located at currentsupport
            meshHalfEdgePtr  nextSupportTriangle = std::make_shared<MeshHalfEdge>(currentSupport->position);

            //nextsupporttriangle-nextSupport-baseEdge
            MeshHalfEdge::ConnectIntoTriangle(nextSupportTriangle, baseEdge, nextSupport);

            supportTriangle->SetPairing(nextSupportTriangle);

            generatedEdges.push_back(supportTriangle);
            generatedEdges.push_back(nextSupportTriangle);

          

        }




    };
}



