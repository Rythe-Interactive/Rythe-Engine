#pragma once
#include <core/core.hpp>
#include <physics/mesh_splitter_utils/mesh_half_edge.hpp>
#include <rendering/components/renderable.hpp>
namespace legion::physics
{

    struct HalfEdgeFinder
    {
        typedef std::pair<int, int> edgeVertexIndexPair;

        typedef std::map<edgeVertexIndexPair,
            std::shared_ptr< physics::MeshHalfEdge>> VertexIndexToHalfEdgePtr;

        typedef std::shared_ptr<MeshHalfEdge> meshHalfEdgePtr;

        meshHalfEdgePtr currentPtr;

        void InitializePolygons(ecs::entity_handle entity)
        {
            auto rederableHandle = entity.get_component_handle<rendering::renderable>();
            auto [posH, rotH, scaleH] = entity.get_component_handles<transform>();

            if (rederableHandle && posH && rotH && scaleH)
            {
                log::debug("Mesh and Transform found");
                std::vector<std::shared_ptr<MeshHalfEdge>> meshHalfEdges;

                auto renderable = rederableHandle.read();
                auto mesh = renderable.model.get_mesh().get().second;

                
                FindHalfEdge(mesh.indices,mesh.vertices
                    , math::compose(scaleH.read(), rotH.read(), posH.read()), meshHalfEdges);

                log::debug("Mesh vertices {}, Mesh indices {}",mesh.vertices.size(),mesh.indices.size());

                Polygonize(meshHalfEdges);
            }
            else
            {
                log::warn("The given entity does not have a meshHandle!");
            }
            
        }

        void FindHalfEdge
        (std::vector<uint>& indices,std::vector<math::vec3>& vertices,math::mat4 transform,
            std::vector<std::shared_ptr<MeshHalfEdge>>& meshHalfEdges)
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


            currentPtr = meshHalfEdges.at(0);

        }

        void Polygonize(std::vector<std::shared_ptr<MeshHalfEdge>>& halfEdges)
        {

        }

        meshHalfEdgePtr InstantiateEdge(int vertexIndex
            , const std::pair<int, int> uniqueIndexPair
            ,const std::vector<math::vec3>& vertices
            , std::vector<meshHalfEdgePtr>& edgePtrs
            , VertexIndexToHalfEdgePtr& indexToEdgeMap)
        {
            auto firstEdge = std::make_shared<MeshHalfEdge>(vertices[vertexIndex]);

            auto edgeToAdd = UniqueAdd(firstEdge, indexToEdgeMap, uniqueIndexPair);

            edgePtrs.push_back(edgeToAdd);

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

