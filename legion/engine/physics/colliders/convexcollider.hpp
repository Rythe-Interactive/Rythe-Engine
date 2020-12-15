#pragma once

#include <limits>
#include <core/core.hpp>
#include <physics/colliders/physicscollider.hpp>
#include <physics/cube_collider_params.hpp>
#include <physics/halfedgeedge.hpp>
#include <physics/halfedgeface.hpp>
#include <physics/data/convex_convergance_identifier.hpp>
#include <physics/data/physics_manifold.hpp>

namespace legion::physics
{



	class ConvexCollider : public PhysicsCollider
	{
	public:

        ConvexCollider() = default;

        ~ConvexCollider()
        {
            for (auto face : halfEdgeFaces)
            {
                delete face;
            }
        }

        /** @brief Given a physics_contact that has been resolved, use its label and lambdas in order to create a ConvexConverganceIdentifier
       */
        void AddConverganceIdentifier(const physics_contact& contact) override
        {
            converganceIdentifiers.push_back(
                std::make_unique<ConvexConverganceIdentifier>(contact.label, contact.totalLambda,
                    contact.tangent1Lambda, contact.tangent2Lambda, GetColliderID()));
        }

        void CheckCollision(std::shared_ptr<PhysicsCollider> physicsCollider, physics_manifold& manifold) override
        {
            physicsCollider->CheckCollisionWith(this, manifold);
        }

        /** @brief Given a ConvexCollider and a physics_manifold, uses the seperating axis test for each HalfEdgeFace and edge
        * of both ConvexColliders 
        */
        void CheckCollisionWith(ConvexCollider* convexCollider, physics_manifold& manifold) override;


        void PopulateContactPoints(std::shared_ptr<PhysicsCollider> physicsCollider, physics_manifold& manifold) override
        {
            physicsCollider->PopulateContactPointsWith(this, manifold);
        }

        void PopulateContactPointsWith(ConvexCollider* convexCollider, physics_manifold& manifold) override;

        /**@brief Given the current transform of the entity, creates a tight AABB of the collider;
        */
        void UpdateTightAABB(math::mat4 transform)
        {

        }


        /**@brief Does one step of the convex hull generation
         * Just for debug purposes 
         */
        void doStep(legion::core::mesh_handle& mesh)
        {
            ConstructConvexHullWithMesh(mesh);
            ++step;
        }

		/**@brief Constructs a polyhedron-shaped convex hull that encompasses the given mesh.
        * @param meshHandle - The mesh handle to lock the mesh and the mesh to create a hull from
		*/
        void ConstructConvexHullWithMesh(legion::core::mesh_handle& meshHandle)
        {
            // Step 0 - Create inital hull
            if (step == 0)
            {
                auto meshLockPair = meshHandle.get();
                async::readonly_guard* meshLockGuard = new async::readonly_guard(meshLockPair.first);
                legion::core::mesh mesh = meshLockPair.second;

                math::ivec2 indices = convexHullFindOuterIndices(mesh);
                int index0 = indices.x;
                int index1 = indices.y;

                int index2 = convexHullFindIndexClosestToLine(mesh, index0, index1);
                math::vec3 normal = normalize(cross((mesh.vertices.at(index1) - mesh.vertices.at(index0)), (mesh.vertices.at(index2) - mesh.vertices.at(index0))));

                //log::debug("plane normal: {}", normal);

                int index3 = convexHullFindIndexClosestToTriangle(mesh, index0, index1, index2, normal);

                if (index3 == mesh.vertices.size())
                {
                    log::error("Initial hull, index 3 cannot be found!");
                    return;
                }

                //log::debug("Vertices: {} ; {} ; {} ; {}", mesh.vertices.at(index0), mesh.vertices.at(index1), mesh.vertices.at(index2), mesh.vertices.at(index3));

                // map to find the index of a face ptr
                //std::unordered_map<HalfEdgeFace*, int> faceIndexMap;

                // Build the initial hull
#pragma region buildInitalHull
                vertices.push_back(mesh.vertices.at(index0));
                vertices.push_back(mesh.vertices.at(index1));
                vertices.push_back(mesh.vertices.at(index2));
                vertices.push_back(mesh.vertices.at(index3));

                // Face 0 - edges: 0, 1, 2 - vertices: 0, 1, 2
                HalfEdgeEdge* edge0 = new HalfEdgeEdge(vertices.at(0));
                HalfEdgeEdge* edge1 = new HalfEdgeEdge(vertices.at(1));
                HalfEdgeEdge* edge2 = new HalfEdgeEdge(vertices.at(2));
                edge0->setNextAndPrevEdge(edge2, edge1);
                edge1->setNextAndPrevEdge(edge0, edge2);
                edge2->setNextAndPrevEdge(edge1, edge0);
                HalfEdgeFace* face012 = new HalfEdgeFace(edge0, normal);
                // Face 1 - edges: 3, 4, 5 - vertices: 3, 1, 0
                HalfEdgeEdge* edge3 = new HalfEdgeEdge(vertices.at(3));
                HalfEdgeEdge* edge4 = new HalfEdgeEdge(vertices.at(1));
                HalfEdgeEdge* edge5 = new HalfEdgeEdge(vertices.at(0));
                edge3->setNextAndPrevEdge(edge5, edge4);
                edge4->setNextAndPrevEdge(edge3, edge5);
                edge5->setNextAndPrevEdge(edge4, edge3);
                math::vec3 normal310 = normalize(cross((mesh.vertices.at(index1) - mesh.vertices.at(index3)), (mesh.vertices.at(index0) - mesh.vertices.at(index3))));
                HalfEdgeFace* face310 = new HalfEdgeFace(edge3, normal310);
                // Face 2 - edges: 6, 7, 8 - vertices 2, 1, 3
                HalfEdgeEdge* edge6 = new HalfEdgeEdge(vertices.at(2));
                HalfEdgeEdge* edge7 = new HalfEdgeEdge(vertices.at(1));
                HalfEdgeEdge* edge8 = new HalfEdgeEdge(vertices.at(3));
                edge6->setNextAndPrevEdge(edge8, edge7);
                edge7->setNextAndPrevEdge(edge6, edge8);
                edge8->setNextAndPrevEdge(edge7, edge6);
                math::vec3 normal213 = normalize(cross((mesh.vertices.at(index1) - mesh.vertices.at(index2)), (mesh.vertices.at(index3) - mesh.vertices.at(index2))));
                HalfEdgeFace* face213 = new HalfEdgeFace(edge6, normal213);
                // Face 3 - edges: 9, 10, 11 - vertices: 3, 0, 2
                HalfEdgeEdge* edge9 = new HalfEdgeEdge(vertices.at(3));
                HalfEdgeEdge* edge10 = new HalfEdgeEdge(vertices.at(0));
                HalfEdgeEdge* edge11 = new HalfEdgeEdge(vertices.at(2));
                edge9->setNextAndPrevEdge(edge11, edge10);
                edge10->setNextAndPrevEdge(edge9, edge11);
                edge11->setNextAndPrevEdge(edge10, edge9);
                math::vec3 normal302 = normalize(cross((mesh.vertices.at(index0) - mesh.vertices.at(index3)), (mesh.vertices.at(index2) - mesh.vertices.at(index3))));
                HalfEdgeFace* face302 = new HalfEdgeFace(edge9, normal302);

                edge0->setPairingEdge(edge4);
                edge1->setPairingEdge(edge6);
                edge2->setPairingEdge(edge10);
                edge3->setPairingEdge(edge7);
                edge5->setPairingEdge(edge9);
                edge8->setPairingEdge(edge11);

                // Make sure all the created faces are convex with each other
                // Also 'repair' them when they are not convex
                HalfEdgeFace::makeNormalsConvexWithFace(*face012, *face310);
                HalfEdgeFace::makeNormalsConvexWithFace(*face012, *face213);
                HalfEdgeFace::makeNormalsConvexWithFace(*face012, *face302);

                HalfEdgeFace::makeNormalsConvexWithFace(*face310, *face213);
                HalfEdgeFace::makeNormalsConvexWithFace(*face310, *face302);

                HalfEdgeFace::makeNormalsConvexWithFace(*face213, *face302);

                halfEdgeFaces.push_back(face012);
                halfEdgeFaces.push_back(face310);
                halfEdgeFaces.push_back(face213);
                halfEdgeFaces.push_back(face302);
                faceIndexMap.emplace(halfEdgeFaces[0], 0);
                faceIndexMap.emplace(halfEdgeFaces[1], 1);
                faceIndexMap.emplace(halfEdgeFaces[2], 2);
                faceIndexMap.emplace(halfEdgeFaces[3], 3);
#pragma endregion buildInitalHull
                // End of building the initial hull

                for (int i = 0; i < mesh.vertices.size(); ++i)
                {
                    bool skipVert = false;
                    for (int j = 0; j < vertices.size(); ++j)
                        if (mesh.vertices[i] == vertices[j]) skipVert = true;
                    if (skipVert) continue;

                    toBeSorted.push_back(mesh.vertices.at(i));
                }

                delete meshLockGuard;
                meshLockGuard = nullptr;
                // From here the mesh may no longer be used

            }
            // end of Step 0 - Create inital hull

            // Step 1 and up, add a vert to the hull
            if (step == 0) return;
            while (looped < step)
            {
                log::debug("------------------------------------------------\n\t\t\t\t\t\tStarting loop {}", looped);

                /*for (int i = 0; i < toBeSorted.size(); ++i)
                {
                    log::debug("ToBeSorted {} @ {}", i, toBeSorted.at(i));
                }*/

                if (toBeSorted.size() == 0)
                {
                    //log::debug("To be sorted is empty");
                    break;
                }

                faceVertMap.clear();
                faceVertMap = convexHullMatchVerticesToFace(toBeSorted);

                // Find the largest distance form vert to its face
                // That vert will be added to the hull
                float largestDistance = 0;
                size_type faceIndex = 0;
                size_type vertIndex = 0;
                for (size_type i = 0; i < faceVertMap.size(); ++i)
                {
                    for (size_type j = 0; j < faceVertMap.at(i).size(); ++j)
                    {
                        //log::debug("faceVertMap[{}][{}] = {}", i, j, faceVertMap.at(i).at(j));
                        float dist = math::pointToTriangle(faceVertMap.at(i).at(j), halfEdgeFaces.at(i)->startEdge->edgePosition, halfEdgeFaces.at(i)->startEdge->nextEdge->edgePosition, halfEdgeFaces.at(i)->startEdge->prevEdge->edgePosition, halfEdgeFaces.at(i)->normal);
                        //log::debug("distance: {}", dist);
                        if (dist > largestDistance)
                        {
                            largestDistance = dist;
                            faceIndex = i;
                            vertIndex = j;
                        }
                    }
                }
                //log::debug("Largest distance for vert: {}, face: {}", faceVertMap.at(faceIndex).at(vertIndex), faceIndex);

                std::deque<HalfEdgeEdge*> edges;
                // The selected vert needs to be merged into the hull
                convexHullConstructHorizon(faceVertMap.at(faceIndex).at(vertIndex), *halfEdgeFaces.at(faceIndex), edges);
                log::debug("VERT TO MERGE: {}", faceVertMap.at(faceIndex).at(vertIndex));
                auto printStuffs = [](HalfEdgeEdge* edge)
                {
                    log::debug("{}", edge->edgePosition);
                };
                log::debug("Matched face: ");
                halfEdgeFaces.at(faceIndex)->forEachEdge(printStuffs);
                log::debug("FOUND {} EDGES", edges.size());

                HalfEdgeEdge* firstEdge = nullptr;
                HalfEdgeEdge* pairingEdge = nullptr;
                std::unordered_map<HalfEdgeFace*, HalfEdgeEdge*> facesToBeDeleted;
                std::vector<HalfEdgeFace*> createdFaces;
                // Create new faces for each edge
                for (int i = 0; i < edges.size(); ++i)
                {
                    log::debug("Edge [{}]: from {}, to {}", i, edges.at(i)->edgePosition, edges.at(i)->nextEdge->edgePosition);
                    //log::debug("Pairing Edge from {}, to {}", edges.at(i)->pairingEdge->edgePosition, edges.at(i)->pairingEdge->nextEdge->edgePosition);
                    facesToBeDeleted.try_emplace(edges.at(i)->face, edges.at(i));
                    // We need to create a copy of the edge edges.at(i), because we still need to delete the old face and old edges later
                    HalfEdgeEdge* edge0 = new HalfEdgeEdge(edges.at(i)->edgePosition);
                    HalfEdgeEdge* edge1 = new HalfEdgeEdge(edges.at(i)->nextEdge->edgePosition); // Tail of edge
                    HalfEdgeEdge* edge2 = new HalfEdgeEdge(faceVertMap.at(faceIndex).at(vertIndex)); // Vertex positon 
                    //log::debug("\tCreated edges @: {}, {}, {}", edge0->edgePosition, edge1->edgePosition, edge2->edgePosition);
                    edge0->setNextAndPrevEdge(edge2, edge1);
                    edge0->setPairingEdge(edges.at(i)->pairingEdge);
                    //log::debug("Adjescent edge from {}, to {}, pairing: {} to {}", edge0->edgePosition, edge0->nextEdge->edgePosition, edge0->pairingEdge->edgePosition, edge0->pairingEdge->nextEdge->edgePosition);
                    edge1->setNextAndPrevEdge(edge0, edge2);
                    edge2->setNextAndPrevEdge(edge1, edge0);
                    if (i == edges.size() - 1)
                    {
                        //log::debug("\tEdge2 @ {} pairing {}", edge2->edgePosition, firstEdge->edgePosition);
                        edge1->setPairingEdge(firstEdge);
                    }
                    if (i == 0)
                    {
                        //log::debug("\tFirst pairing edge set to: {}", edge2->edgePosition);
                        firstEdge = edge2;
                    }
                    else
                    {
                        //log::debug("\tEdge2 @ {} pairing {}", edge1->edgePosition, pairingEdge->edgePosition);
                        edge2->setPairingEdge(pairingEdge);
                    }
                    pairingEdge = edge1;

                    // Calculate normal
                    math::vec3 normal = math::normalize(math::cross(edge1->edgePosition - edge0->edgePosition, edge2->edgePosition - edge0->edgePosition));
                    HalfEdgeFace* face = new HalfEdgeFace(edge0, normal);
                    face->setFaceForAllEdges();
                    //halfEdgeFaces.push_back(face);
                    createdFaces.push_back(face);
                }
                vertices.push_back(faceVertMap.at(faceIndex).at(vertIndex));

                math::vec3 addedVert = faceVertMap.at(faceIndex).at(vertIndex);
                toBeSorted.clear();
                for (size_type i = 0; i < faceVertMap.size(); ++i)
                {
                    for (size_type j = 0; j < faceVertMap.at(i).size(); ++j)
                    {
                        // Make sure we do not add the vert to the list that we just added to the hull
                        if(faceVertMap.at(i).at(j) != addedVert)
                            toBeSorted.push_back(faceVertMap.at(i).at(j));
                    }
                }

                //log::debug("Face count: {}", halfEdgeFaces.size());
                // Delete the edges and faces
                for (auto& f : facesToBeDeleted)
                {
                    //f.first->deleteEdges();
                    halfEdgeFaces.erase(std::remove(halfEdgeFaces.begin(), halfEdgeFaces.end(), f.first), halfEdgeFaces.end());
                    faceIndexMap.erase(faceIndexMap.find(f.first));
                    delete f.first;
                }
                //log::debug("Face count: {}", halfEdgeFaces.size());

                for (int i = createdFaces.size() - 1; i >= 0; --i)
                {
                    log::debug("Created face {}, {}, {}, {}", i, createdFaces.at(i)->startEdge->edgePosition, createdFaces.at(i)->startEdge->nextEdge->edgePosition, createdFaces.at(i)->startEdge->prevEdge->edgePosition);
                }

                convexHullMergeFaces(createdFaces);

                // Make sure all the normals are correct
                // It seems that making the faces convex with each other is not necesarry in 3D
                for (int i = 0; i < createdFaces.size(); ++i)
                {
                    faceIndexMap.emplace(createdFaces.at(i), halfEdgeFaces.size());
                    halfEdgeFaces.push_back(createdFaces.at(i));
                    //for (int j = i + 1; j < createdFaces.size(); ++j)
                    //{
                    //    bool convexity = !HalfEdgeFace::makeNormalsConvexWithFace(*createdFaces.at(i), *createdFaces.at(j));
                    //    //log::debug("Convexity for {} and {}: {}", i, j, convexity);
                    //}
                }

                ++looped;
            }


            //AssertEdgeValidity();
        }

		/**@brief Constructs a box-shaped convex hull that encompasses the given mesh.
		*/
        void ConstructBoxWithMesh()
        {

        }

		/**@brief Constructs a box-shaped convex hull based on the given parameters.
		 * @param cubeParams The parameters that will be used to create the box.
		*/
        void CreateBox(const cube_collider_params& cubeParams)
        {
            //initialize box vertices
            float halfWidth = cubeParams.width / 2;
            float halfBreath = cubeParams.breadth / 2;
            float halfHeight = cubeParams.height / 2;

            // BOTTOM FACE          // TOP FACE

            //a ----------- b       //e ----------- f
            //||           ||       //||           ||
            //||           ||       //||           ||
            //||           ||       //||           ||
            //||           ||       //||           ||                                              
            //c ----------- d       //g ----------- h

            /*a*/ math::vec3 minVertexPlusBreadth = math::vec3(-halfWidth, -halfHeight, halfBreath);
            /*b*/ math::vec3 minVertexPlusWidthPlusBreadth = math::vec3(halfWidth, -halfHeight, halfBreath);
            /*c*/ math::vec3 minVertex = math::vec3(-halfWidth, -halfHeight, -halfBreath);
            /*d*/ math::vec3 minVertexPlusWidth = math::vec3(halfWidth, -halfHeight, -halfBreath);

            /*e*/ math::vec3 maxVertexMinusWidth = math::vec3(-halfWidth, halfHeight, halfBreath);
            /*f*/ math::vec3 maxVertex = math::vec3(halfWidth, halfHeight, halfBreath);
            /*g*/ math::vec3 maxVertexMinusWidthMinusBreadth = math::vec3(-halfWidth, halfHeight, -halfBreath);
            /*h*/ math::vec3 maxVertexMinusBreadth = math::vec3(halfWidth, halfHeight, -halfBreath);

            //the HalfEdgeEdge only needs a ptr to the vertex  
            vertices.push_back(minVertexPlusBreadth);
            vertices.push_back(minVertexPlusWidthPlusBreadth);
            vertices.push_back(minVertex);
            vertices.push_back(minVertexPlusWidth);
            vertices.push_back(maxVertexMinusWidth);
            vertices.push_back(maxVertex);
            vertices.push_back(maxVertexMinusWidthMinusBreadth);
            vertices.push_back(maxVertexMinusBreadth);

            math::vec3 a = vertices.at(0);
            math::vec3 b = vertices.at(1);
            math::vec3 c = vertices.at(2);
            math::vec3 d = vertices.at(3);
            math::vec3 e = vertices.at(4);
            math::vec3 f = vertices.at(5);
            math::vec3 g = vertices.at(6);
            math::vec3 h = vertices.at(7);

            for (auto& vertex : vertices)
            {
                vertex += cubeParams.offset;
            }

            //note: each edge carries adjacency information. (for example, an edge 'eg' must know its edge pair 'ge').
            //This is why each edge must be declared explicitly.

            //each face also has an id. It is mostly used for debugging reasons and will be removed when it is no longer needed.

            //[1] create face eghf

            HalfEdgeEdge* eg = new HalfEdgeEdge(e);
            HalfEdgeEdge* gh = new HalfEdgeEdge(g);
            HalfEdgeEdge* hf = new HalfEdgeEdge(h);
            HalfEdgeEdge* fe = new HalfEdgeEdge(f);

            eg->setNextAndPrevEdge(fe, gh);
            gh->setNextAndPrevEdge(eg, hf);
            hf->setNextAndPrevEdge(gh, fe);
            fe->setNextAndPrevEdge(hf, eg);

            HalfEdgeFace* eghf = new HalfEdgeFace(eg, math::vec3(0, 1, 0));
            halfEdgeFaces.push_back(eghf);
            //eghf->id = " eghf";

            //[2] create face hgcd

            HalfEdgeEdge* hg = new HalfEdgeEdge(h);
            HalfEdgeEdge* gc = new HalfEdgeEdge(g);
            HalfEdgeEdge* cd = new HalfEdgeEdge(c);
            HalfEdgeEdge* dh = new HalfEdgeEdge(d);

            hg->setNextAndPrevEdge(dh, gc);
            gc->setNextAndPrevEdge(hg, cd);
            cd->setNextAndPrevEdge(gc, dh);
            dh->setNextAndPrevEdge(cd, hg);

            HalfEdgeFace* hgcd = new HalfEdgeFace(hg, math::vec3(0, 0, -1));
            halfEdgeFaces.push_back(hgcd);
            //hgcd->id = "hgcd";

            //[3] create face fhdb

            HalfEdgeEdge* fh = new HalfEdgeEdge(f);
            HalfEdgeEdge* hd = new HalfEdgeEdge(h);
            HalfEdgeEdge* db = new HalfEdgeEdge(d);
            HalfEdgeEdge* bf = new HalfEdgeEdge(b);

            fh->setNextAndPrevEdge(bf, hd);
            hd->setNextAndPrevEdge(fh, db);
            db->setNextAndPrevEdge(hd, bf);
            bf->setNextAndPrevEdge(db, fh);

            HalfEdgeFace* fhdb = new HalfEdgeFace(fh, math::vec3(1, 0, 0));
            halfEdgeFaces.push_back(fhdb);
            //fhdb->id = "fhdb";

            //[4] create face efba

            HalfEdgeEdge* ef = new HalfEdgeEdge(e);
            HalfEdgeEdge* fb = new HalfEdgeEdge(f);
            HalfEdgeEdge* ba = new HalfEdgeEdge(b);
            HalfEdgeEdge* ae = new HalfEdgeEdge(a);

            ef->setNextAndPrevEdge(ae, fb);
            fb->setNextAndPrevEdge(ef, ba);
            ba->setNextAndPrevEdge(fb, ae);
            ae->setNextAndPrevEdge(ba, ef);

            HalfEdgeFace* efba = new HalfEdgeFace(ef, math::vec3(0, 0, 1));
            halfEdgeFaces.push_back(efba);
            //efba->id = "efba";

            //[5] create face geac

            HalfEdgeEdge* ge = new HalfEdgeEdge(g);
            HalfEdgeEdge* ea = new HalfEdgeEdge(e);
            HalfEdgeEdge* ac = new HalfEdgeEdge(a);
            HalfEdgeEdge* cg = new HalfEdgeEdge(c);

            ge->setNextAndPrevEdge(cg, ea);
            ea->setNextAndPrevEdge(ge, ac);
            ac->setNextAndPrevEdge(ea, cg);
            cg->setNextAndPrevEdge(ac, ge);

            HalfEdgeFace* geac = new HalfEdgeFace(ge, math::vec3(-1, 0, 0));
            halfEdgeFaces.push_back(geac);
            //geac->id = "geac";

            //[6] create face abdc

            HalfEdgeEdge* ab = new HalfEdgeEdge(a);
            HalfEdgeEdge* bd = new HalfEdgeEdge(b);
            HalfEdgeEdge* dc = new HalfEdgeEdge(d);
            HalfEdgeEdge* ca = new HalfEdgeEdge(c);

            ab->setNextAndPrevEdge(ca, bd);
            bd->setNextAndPrevEdge(ab, dc);
            dc->setNextAndPrevEdge(bd, ca);
            ca->setNextAndPrevEdge(dc, ab);

            HalfEdgeFace* abdc = new HalfEdgeFace(ab, math::vec3(0, -1, 0));
            halfEdgeFaces.push_back(abdc);
            //abdc->id = "abdc";


            //manually connect each edge to its pair

            //eghf                  //hgcd
            eg->pairingEdge = ge;   hg->pairingEdge = gh;
            gh->pairingEdge = hg;   gc->pairingEdge = cg;
            hf->pairingEdge = fh;   cd->pairingEdge = dc;
            fe->pairingEdge = ef;   dh->pairingEdge = hd;

            //fhdb                   //efba
            fh->pairingEdge = hf;    ef->pairingEdge = fe;
            hd->pairingEdge = dh;    fb->pairingEdge = bf;
            db->pairingEdge = bd;    ba->pairingEdge = ab;
            bf->pairingEdge = fb;    ae->pairingEdge = ea;

            //geac                  //abdc
            ge->pairingEdge = eg;   ab->pairingEdge = ba;
            ea->pairingEdge = ae;   bd->pairingEdge = db;
            ac->pairingEdge = ca;   dc->pairingEdge = cd;
            cg->pairingEdge = gc;   ca->pairingEdge = ac;

            
            //initialize the ID of the edges, this is done mostly for debugging reasons and will be removed when it
            //is no longer needed
            
            //eghf           //hgcd
            eg->id = "eg";   hg->id = "hg";
            gh->id = "gh";   gc->id = "gc";
            hf->id = "hf";   cd->id = "cd";
            fe->id = "fe";   dh->id = "dh";

            //fhdb            //efba
            fh->id = "fh";    ef->id = "ef";
            hd->id = "hd";    fb->id = "fb";
            db->id = "db";    ba->id = "ba";
            bf->id = "bf";    ae->id = "ae";

            //geac           //abdc
            ge->id = "ge";   ab->id = "ab";
            ea->id = "ea";   bd->id = "bd";
            ac->id = "ac";   dc->id = "dc";
            cg->id = "cg";   ca->id = "ca";
          
            //check if halfEdge data structure was initialized correctly. this will be commented when I know it always works
            AssertEdgeValidity();

        }

        std::vector<HalfEdgeFace*>& GetHalfEdgeFaces() override
        {
            return halfEdgeFaces;
        }

        const std::vector<math::vec3>& GetVertices() const
        {
            return vertices;
        }

        void AssertEdgeValidity()
        {
            auto assertFunc = [](HalfEdgeEdge* edge)
            {
                assert(edge->nextEdge);
                assert(edge->prevEdge);
                assert(edge->pairingEdge);
                assert(edge->face);
                
            };

            for (auto& face : halfEdgeFaces)
            {
                face->forEachEdge(assertFunc);
            }
        }
        

	private:

        HalfEdgeFace* instantiateMeshFace(const std::vector<math::vec3*>& vertices, const math::vec3& faceNormal)
        {
            if (vertices.size() == 0) { return nullptr; }

            std::vector<HalfEdgeEdge*> faceEdges;

            for (const auto vert : vertices)
            {
                faceEdges.push_back(new HalfEdgeEdge(*vert));
            }

            for (size_t i = 0; i < faceEdges.size(); i++)
            {
                int prevIndex = (i - 1) >= 0 ? (i - 1) : faceEdges.size() - 1;
                int afterIndex = (i + 1) < faceEdges.size() ? (i + 1) : 0;

                HalfEdgeEdge* prevEdge = faceEdges.at(prevIndex);
                HalfEdgeEdge* nextEdge = faceEdges.at(afterIndex);

                faceEdges.at(i)->setNextAndPrevEdge(prevEdge, nextEdge);

            }

            return new HalfEdgeFace(faceEdges.at(0), faceNormal);
        }

        /**@brief Function to find the outer two indices with the largest distance from the mesh
         * @brief The indices are found by constructing a virtual box around the mesh and checking all the outer vertices with each other for the greatest distance
         * @param mesh - Mesh to find the outer vertices for
         * @return ivec2 containing the outer vertices. x is the first index, y is the second.
         */
        math::ivec2 convexHullFindOuterIndices(core::mesh& mesh)
        {
            // Step 1 - create a box around the mesh

            math::vec3 first = mesh.vertices.at(0);
            // stores outer vertices in the following order:
            // min x, max x, min y, max y, min z, max z
            // The array is initialized with the first vertex
            float outer[] = { first.x, first.x, first.y, first.y, first.z, first.z };
            int indices[] = { 0,0,0,0,0,0 };
            for (int i = 1; i < mesh.vertices.size(); ++i)
            {
                if (mesh.vertices.at(i).x < outer[0])
                {
                    outer[0] = mesh.vertices.at(i).x;
                    indices[0] = i;
                }
                else if (mesh.vertices.at(i).x > outer[1])
                {
                    outer[1] = mesh.vertices.at(i).x;
                    indices[1] = i;
                }
                if (mesh.vertices.at(i).y < outer[2])
                {
                    outer[2] = mesh.vertices.at(i).y;
                    indices[2] = i;
                }
                else if (mesh.vertices.at(i).y > outer[3])
                {
                    outer[3] = mesh.vertices.at(i).y;
                    indices[3] = i;
                }
                if (mesh.vertices.at(i).z < outer[4])
                {
                    outer[4] = mesh.vertices.at(i).z;
                    indices[4] = i;
                }
                else if (mesh.vertices.at(i).z > outer[5])
                {
                    outer[5] = mesh.vertices.at(i).z;
                    indices[5] = i;
                }
            }

            log::debug("Outer: x{}, x{}, y{}, y{}, z{}, z{}", outer[0], outer[1], outer[2], outer[3], outer[4], outer[5]);

            // Check if it is a plane, min == max

            float largestDistance = 0;
            int index0;
            int index1;
            // Find the largest distance between two vertices in the box
            for (int i = 0; i < 6; ++i)
            {
                for (int j = i; j < 6; ++j)
                {
                    float dist = distance(mesh.vertices[indices[i]], mesh.vertices[indices[j]]);
                    if (dist > largestDistance)
                    {
                        index0 = indices[i];
                        index1 = indices[j];
                        largestDistance = dist;
                    }
                }
            }
            return math::ivec2(index0, index1);
        }

        /**@brief Function to find the index in a mesh which is closest to a line between two vertices in the mesh
         * @param mesh - The mesh with the vertices
         * @param lineStartIndex - The index of the vertex where the line starts
         * @param lineEndIndex - The index of the vertex at the end of the line
         * @return The index of the vertex with the largest distance from the line, mesh.vertices.size() is returned if a such a point cannot be found
         */
        size_type convexHullFindIndexClosestToLine(core::mesh& mesh, size_type lineStartIndex, size_type lineEndIndex)
        {
            math::vec3 linedir = normalize(mesh.vertices[lineEndIndex] - mesh.vertices[lineStartIndex]);
            //log::debug("Line: {} to {}", mesh.vertices[lineStartIndex], mesh.vertices[lineEndIndex]);

            float largestDistance = 0;
            // Save the index which is furthest from the line between the mesh vertices at lineStartIndex and lineEndIndex
            size_type index = mesh.vertices.size();

            // Possible speed up.
            // Check if mesh.vertices.at(i) is equal to line start coord of line end coord. Such an if check is possibly faster than the math
            for (int i = 0; i < mesh.vertices.size(); ++i)
            {
                if (i == lineStartIndex || i == lineEndIndex) continue;
                float dist = math::pointToLineSegment(mesh.vertices.at(i), mesh.vertices.at(lineStartIndex), mesh.vertices.at(lineEndIndex));
                if (dist > largestDistance && dist > 0)
                {
                    largestDistance = dist;
                    index = i;
                }
            }
            //log::debug("Closest index to line: {} ; {}, dist: {}", index, mesh.vertices.at(index), largestDistance);
            return index;
        }

        /**@brief Function to find the index in a mesh which is closest to the plane between triangle plane indices
         * @param mesh - The mesh with the vertices
         * @param planeIndex0 - First triangle plane vertex index
         * @param planeIndex1 - Second triangle plane vertex index
         * @param planeIndex2 - Third triangle plane vertex index
         * @param normal - The normal of the plane vertex index
         * @return The index of the vertex with the largest distance from the plane, mesh.vertices.size() is returned if a such a point cannot be found
         */
        size_type convexHullFindIndexClosestToTriangle(core::mesh& mesh, size_type planeIndex0, size_type planeIndex1, size_type planeIndex2, math::vec3 normal)
        {
            float largestDistance = 0;
            // Save the index which is furthest from the plane between the mesh vertices at the plane indices
            int index = mesh.vertices.size();
            // Possible speed up.
            // Check if mesh.vertices.at(i) is equal to line start coord of line end coord. Such an if check is possibly faster than the math
            for (int i = 0; i < mesh.vertices.size(); ++i)
            {
                if (i == planeIndex0 || i == planeIndex1 || i == planeIndex2) continue;
                float dist = abs(math::pointToTriangle(mesh.vertices.at(i), mesh.vertices.at(planeIndex0), mesh.vertices.at(planeIndex1), mesh.vertices.at(planeIndex2), normal));
                if (dist > largestDistance && dist > 0)
                {
                    largestDistance = dist;
                    index = i;
                }
            }
            return index;
        }

        /**@brief Function to match unused mesh vertices to convex hull faces
         * @brief A vertex is matched to its closest face 
         * @brief Vertices that are inside all faces are not stored
         * @param vertices - The vertices that are to be matched 
         * @return A list of a list of indeces in the vertices list.
         * std::deque<std::vector<int>>.at(index) gets the list of vertices that are matched to face at targeted index in the halfEdgeFaces list
         * std::vector<int>.at(index) gets the vertex
         */

        // TODO / TO DO / ERROR
        // THE ERROR IN MY CODE THAT DOES NOT GENERATE THE HULL CORRECTLY IS IN THIS PART
        // OR AT LEAST, THE LAST VERT (FOR THE CURRENT MESH) DOES NOT GET MATCHED WITH THE CORRECT FACE
        // EXPECTATION OF ERROR:
        //      I EXPECT THAT BECAUSE IT TAKES THE DISTANCE TOWARDS A TRIANGLE IT HAS A WRONG DISTANCE WHEN IT COMES TO FACES WITH MORE THAN 3 SIDES

        std::vector<std::vector<math::vec3>> convexHullMatchVerticesToFace(std::vector<math::vec3>& vertices)
        {
            std::vector<std::vector<math::vec3>> map = std::vector<std::vector<math::vec3>>(halfEdgeFaces.size());

            for (size_type i = 0; i < vertices.size(); ++i)
            {
                float smallestDistance = std::numeric_limits<float>::max();
                int faceIndex = -1;
                math::vec3 faceVerts[] = { math::vec3(0,0,0), math::vec3(0,0,0), math::vec3(0,0,0)};
                log::debug("\n\n\n\t\t\t\tVert {} @ {}", i, vertices.at(i));
                for (size_type f = 0; f < halfEdgeFaces.size(); ++f)
                {
                    std::vector<math::vec3> points;
                    log::debug("Face @ ");
                    auto collectPoints = [&points](HalfEdgeEdge* edge)
                    {
                        points.push_back(edge->edgePosition);
                        log::debug("{}", edge->edgePosition);
                    };
                    halfEdgeFaces.at(f)->forEachEdge(collectPoints);
                    float distToPlane = math::pointToPlane(vertices.at(i), points.at(0), halfEdgeFaces.at(f)->normal);
                    bool projected = math::projectedPointInPolygon(vertices.at(i), points, halfEdgeFaces.at(f)->normal, halfEdgeFaces.at(f)->centroid);
                    log::debug("PlaneDist: {} && Projected: {}", distToPlane, projected);
                    if (distToPlane > 0 && projected)
                    {
                        log::debug("Did the thing!----------------------------------------------------Did the thing!");
                        // Get the distances to the face
                        HalfEdgeEdge* startEdge = halfEdgeFaces.at(f)->startEdge;
                        float distance = math::pointToTriangle(vertices.at(i), startEdge->edgePosition, startEdge->nextEdge->edgePosition, startEdge->prevEdge->edgePosition, halfEdgeFaces.at(f)->normal);
                        if (distance > 0 && distance < smallestDistance)
                        {
                            faceIndex = f;
                            smallestDistance = distance;
                            faceVerts[0] = startEdge->edgePosition;
                            faceVerts[1] = startEdge->nextEdge->edgePosition;
                            faceVerts[2] = startEdge->prevEdge->edgePosition;
                        }
                    }
                }
                if (faceIndex >= 0)
                {
                    map[faceIndex].push_back(vertices.at(i));
                }
            }
            return map;
        }


        void convexHullConstructHorizon(math::vec3 vert, HalfEdgeFace& face, std::deque<HalfEdgeEdge*>& edges, HalfEdgeEdge* originEdge = nullptr, HalfEdgeFace* originFace = nullptr)
        {
            if (&face == originFace) return;
            if (originFace == nullptr) originFace = &face;
            HalfEdgeEdge* start = face.startEdge->prevEdge;
            HalfEdgeEdge* edge = face.startEdge->prevEdge;

            // Loop through all the edges of this face
            do
            {
                if (originEdge == edge)
                {
                    edge = edge->nextEdge;
                    continue;
                }
                // Cross the edge
                HalfEdgeFace* crossedFace = edge->pairingEdge->face;

                // If the vert is above the face, the face is part of the horizon
                if (math::pointToPlane(vert, crossedFace->startEdge->edgePosition, crossedFace->normal) > 0.0f)
                {
                    // Plane is part of horizon
                    convexHullConstructHorizon(vert, *crossedFace, edges, edge->pairingEdge, originFace);
                }
                else
                {
                    // Plane is not part of horizon
                    // Therefore the crossed edge was a horizon edge
                    edges.push_back(edge);
                }

                edge = edge->nextEdge;
            } while (edge != start);
        }

        void convexHullMergeFaces(std::vector<HalfEdgeFace*>& createdFaces)
        {
            if (looped == 4)
            {
                log::debug("Not merging!");
                return;
            }

            for (int i = createdFaces.size() - 1; i >= 0; --i)
            {
                log::debug("Created face {}, {}, {}, {}", i, createdFaces.at(i)->startEdge->edgePosition, createdFaces.at(i)->startEdge->nextEdge->edgePosition, createdFaces.at(i)->startEdge->prevEdge->edgePosition);
            }

            auto printPositions = [](HalfEdgeEdge* edge)
            {
                log::debug("{}", edge->edgePosition);
            };

            bool looping = false;
            // Merge faces with coplanerity
            for (int i = createdFaces.size() - 1; i >= 0; --i)
            {
                for (int j = 0; j < halfEdgeFaces.size(); ++j)
                {
                    // Do not check if createdFace is halfEdgeFace, because the createdFaces are not yet added to the halfEdgeFaces list
                    /*log::debug(createdFaces.at(i)->startEdge->edgePosition);
                    log::debug(halfEdgeFaces.at(j)->startEdge->edgePosition);*/
                    HalfEdgeFace::face_angle_relation relation0 = createdFaces.at(i)->getAngleRelation(*halfEdgeFaces.at(j));
                    //HalfEdgeFace::face_angle_relation relation1 = halfEdgeFaces.at(j)->getAngleRelation(*createdFaces.at(i));
                    //log::debug("Relation between faces: {}", HalfEdgeFace::to_string(relation0));
                    if (relation0 == HalfEdgeFace::face_angle_relation::coplaner)
                    {
                        HalfEdgeEdge* centerEdge = HalfEdgeFace::findMiddleEdge(*halfEdgeFaces.at(j), *createdFaces.at(i));
                        if (centerEdge != nullptr)
                        {
                            log::debug("Merging Faces");
                            HalfEdgeFace* face = HalfEdgeFace::mergeFaces(*centerEdge);
                            assert(face == halfEdgeFaces.at(j));
                            face->forEachEdge(printPositions);
                            createdFaces.erase(createdFaces.begin() + i);
                            looping = true;
                            //log::debug("Merging faces");
                            break;
                        }
                        //else log::debug("Center edge is nullptr");
                    }
                }
            }

            for (int i = createdFaces.size() - 1; i >= 0; --i)
            {
                log::debug("Created face {}, {}, {}, {}", i, createdFaces.at(i)->startEdge->edgePosition, createdFaces.at(i)->startEdge->nextEdge->edgePosition, createdFaces.at(i)->startEdge->prevEdge->edgePosition);
            }

            while (looping)
            {
                looping = false;
                log::debug("Looping through faces");

                for (int i = createdFaces.size() - 1; i >= 0; --i)
                {
                    for (int j = 0; j < halfEdgeFaces.size(); ++j)
                    {
                        // Do not check if createdFace is halfEdgeFace, because the createdFaces are not yet added to the halfEdgeFaces list
                        /*log::debug(createdFaces.at(i)->startEdge->edgePosition);
                        log::debug(halfEdgeFaces.at(j)->startEdge->edgePosition);*/
                        HalfEdgeFace::face_angle_relation relation0 = createdFaces.at(i)->getAngleRelation(*halfEdgeFaces.at(j));
                        //HalfEdgeFace::face_angle_relation relation1 = halfEdgeFaces.at(j)->getAngleRelation(*createdFaces.at(i));
                        log::debug("Relation between faces: {}", HalfEdgeFace::to_string(relation0));
                        if (relation0 == HalfEdgeFace::face_angle_relation::coplaner)
                        {
                            HalfEdgeEdge* centerEdge = HalfEdgeFace::findMiddleEdge(*halfEdgeFaces.at(j), *createdFaces.at(i));
                            if (centerEdge != nullptr)
                            {
                                log::debug("Merging Faces");
                                HalfEdgeFace* face = HalfEdgeFace::mergeFaces(*centerEdge);
                                assert(face == halfEdgeFaces.at(j));
                                createdFaces.erase(createdFaces.begin() + i);
                                looping = true;
                                //log::debug("Merging faces");
                                break;
                            }
                            else log::debug("Center edge is nullptr");
                        }
                    }
                }
            }
        }

        std::vector<math::vec3> vertices;
        std::vector<HalfEdgeFace*> halfEdgeFaces;

        // Convex hull generation debug stuffs
        int step = 0;
        std::vector<math::vec3> toBeSorted;
        std::vector<std::vector<math::vec3>> faceVertMap;
        std::unordered_map<HalfEdgeFace*, int> faceIndexMap;
        int looped = 0;

        //feature id container
	};
}


