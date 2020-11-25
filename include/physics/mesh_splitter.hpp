#pragma once
#include <core/core.hpp>
#include <physics/mesh_splitter_utils/mesh_half_edge.hpp>
#include <physics/mesh_splitter_utils/splittable_polygon.h>
#include <rendering/components/renderable.hpp>
namespace legion::physics
{

    struct MeshSplitter
    {
        typedef std::pair<int, int> edgeVertexIndexPair;

        typedef std::map<edgeVertexIndexPair,
            std::shared_ptr< physics::MeshHalfEdge>> VertexIndexToHalfEdgePtr;

        typedef std::shared_ptr<MeshHalfEdge> meshHalfEdgePtr;
        typedef std::shared_ptr<SplittablePolygon> SplittablePolygonPtr;

        meshHalfEdgePtr currentPtr;

        std::vector<std::shared_ptr< SplittablePolygon>> meshPolygons;

        /** @brief Creates a Half-Edge Data structure around the mesh and 
        * 
        */
        void InitializePolygons(ecs::entity_handle entity)
        {
            auto rederableHandle = entity.get_component_handle<rendering::renderable>();
            auto [posH, rotH, scaleH] = entity.get_component_handles<transform>();

            if (rederableHandle && posH && rotH && scaleH)
            {
                log::debug("Mesh and Transform found");
                std::queue<meshHalfEdgePtr> meshHalfEdges;

                auto renderable = rederableHandle.read();
                auto mesh = renderable.model.get_mesh().get().second;

                const math::mat4 transform = math::compose(scaleH.read(), rotH.read(), posH.read());
                FindHalfEdge(mesh.indices,mesh.vertices
                    , transform, meshHalfEdges);

                BFSPolygonize(meshHalfEdges,transform);

                log::debug("Mesh vertices {}, Mesh indices {}",mesh.vertices.size(),mesh.indices.size());

                
            }
            else
            {
                log::warn("The given entity does not have a meshHandle!");
            }
            
        }

        void FindHalfEdge
        (std::vector<uint>& indices,std::vector<math::vec3>& vertices,math::mat4 transform,
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

            for (int i = 0; i < vertices.size(); ++i)
            {
                math::vec3 position = vertices.at(i);

                bool isVectorSeen = false;

                //have we found this vector before?
                for (int j = 0; j < uniquePositions.size(); j++)
                {
                    math::vec3 transformedPos = transform * math::vec4(position,1);
                    math::vec3 uniqueTransformedPos = transform * math::vec4(uniquePositions[j],1);

                    float dist = math::distance(uniqueTransformedPos, transformedPos);

                    if (math::epsilonEqual<float>(dist, 0.0f,math::epsilon<float>()))
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

        void BFSPolygonize(std::queue<meshHalfEdgePtr>& halfEdgeQueue,const math::mat4& transform)
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
                    }

                       
                }

            }

            for (auto edge : edgesNotInPolygon)
            {
                edge->isVisited = false;
                halfEdgeQueue.push(edge);
            }

            polygon = std::make_shared<SplittablePolygon>(edgesInPolygon);
            polygon->calculateLocalCentroid();

            return true;
        }

        meshHalfEdgePtr InstantiateEdge(int vertexIndex
            , const std::pair<int, int> uniqueIndexPair
            ,const std::vector<math::vec3>& vertices
            , std::queue<meshHalfEdgePtr>& edgePtrs
            , VertexIndexToHalfEdgePtr& indexToEdgeMap)
        {
            auto firstEdge = std::make_shared<MeshHalfEdge>(vertices[vertexIndex]);

            auto edgeToAdd = UniqueAdd(firstEdge, indexToEdgeMap, uniqueIndexPair);

            edgePtrs.push(edgeToAdd);

            return edgeToAdd;
        }

        meshHalfEdgePtr UniqueAdd(meshHalfEdgePtr newEdge,VertexIndexToHalfEdgePtr& indexToEdgeMap,  edgeVertexIndexPair uniqueIndexPair)
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


    };

}

