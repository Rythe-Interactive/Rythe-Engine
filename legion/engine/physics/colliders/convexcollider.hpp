#pragma once

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

		/**@brief Constructs a polyhedron-shaped convex hull that encompasses the given mesh.
        * @param meshHandle - The mesh handle to lock the mesh and the mesh to create a hull from
		*/
        void ConstructConvexHullWithMesh(legion::core::mesh_handle& meshHandle)
        {
            auto meshLockPair = meshHandle.get();
            async::readonly_guard guard(meshLockPair.first);
            legion::core::mesh mesh = meshLockPair.second;
            using namespace math;

            ivec2 indices = convexHullFindOuterIndices(mesh);
            int index0 = indices.x;
            int index1 = indices.y;

            log::debug("indices {}, {}", index0, index1);

            int index2 = convexHullFindIndexClosestToLine(mesh, index0, index1);
            log::debug("index2 {}", index2);
            log::debug("Vertices: {} ; {} ; {}", mesh.vertices.at(index0), mesh.vertices.at(index1), mesh.vertices.at(index2));

            vec3 normal = normalize(cross((mesh.vertices.at(index1) - mesh.vertices.at(index0)), (mesh.vertices.at(index2) - mesh.vertices.at(index0))));

            int index3 = convexHullFindIndexClosestToPlane(mesh, index0, index1, index2, normal);

            vertices.push_back(mesh.vertices.at(index0));
            vertices.push_back(mesh.vertices.at(index1));
            vertices.push_back(mesh.vertices.at(index2));
            vertices.push_back(mesh.vertices.at(index3));

            // Face 0 - edges: 0, 1, 2 - vertices: 0, 1, 2
            HalfEdgeEdge* edge0 = new HalfEdgeEdge(&vertices.at(0));
            HalfEdgeEdge* edge1 = new HalfEdgeEdge(&vertices.at(1));
            HalfEdgeEdge* edge2 = new HalfEdgeEdge(&vertices.at(2));
            edge0->setNextAndPrevEdge(edge2, edge1);
            edge1->setNextAndPrevEdge(edge0, edge2);
            edge2->setNextAndPrevEdge(edge1, edge0);
            HalfEdgeFace* face012 = new HalfEdgeFace(edge0, normal);
            // Face 1 - edges: 3, 4, 5 - vertices: 3, 1, 0
            HalfEdgeEdge* edge3 = new HalfEdgeEdge(&vertices.at(3));
            HalfEdgeEdge* edge4 = new HalfEdgeEdge(&vertices.at(1));
            HalfEdgeEdge* edge5 = new HalfEdgeEdge(&vertices.at(0));
            edge3->setNextAndPrevEdge(edge5, edge4);
            edge4->setNextAndPrevEdge(edge3, edge5);
            edge5->setNextAndPrevEdge(edge4, edge3);
            vec3 normal310 = normalize(cross((mesh.vertices.at(index1) - mesh.vertices.at(index3)), (mesh.vertices.at(index0) - mesh.vertices.at(index3))));
            HalfEdgeFace* face310 = new HalfEdgeFace(edge3, normal310);
            // Face 2 - edges: 6, 7, 8 - vertices 2, 1, 3
            HalfEdgeEdge* edge6 = new HalfEdgeEdge(&vertices.at(2));
            HalfEdgeEdge* edge7 = new HalfEdgeEdge(&vertices.at(1));
            HalfEdgeEdge* edge8 = new HalfEdgeEdge(&vertices.at(3));
            edge6->setNextAndPrevEdge(edge8, edge7);
            edge7->setNextAndPrevEdge(edge6, edge8);
            edge8->setNextAndPrevEdge(edge7, edge6);
            vec3 normal213 = normalize(cross((mesh.vertices.at(index1) - mesh.vertices.at(index2)), (mesh.vertices.at(index3) - mesh.vertices.at(index2))));
            HalfEdgeFace* face213 = new HalfEdgeFace(edge6, normal213);
            // Face 3 - edges: 9, 10, 11 - vertices: 3, 0, 2
            HalfEdgeEdge* edge9 = new HalfEdgeEdge(&vertices.at(3));
            HalfEdgeEdge* edge10 = new HalfEdgeEdge(&vertices.at(0));
            HalfEdgeEdge* edge11 = new HalfEdgeEdge(&vertices.at(2));
            edge9->setNextAndPrevEdge(edge11, edge10);
            edge10->setNextAndPrevEdge(edge9, edge11);
            edge11->setNextAndPrevEdge(edge10, edge9);
            vec3 normal302 = normalize(cross((mesh.vertices.at(index0) - mesh.vertices.at(index3)), (mesh.vertices.at(index2) - mesh.vertices.at(index3))));
            HalfEdgeFace* face302 = new HalfEdgeFace(edge6, normal302);

            edge0->setPairingEdge(edge4);
            edge1->setPairingEdge(edge6);
            edge2->setPairingEdge(edge10);
            edge3->setPairingEdge(edge7);
            edge5->setPairingEdge(edge9);
            edge8->setPairingEdge(edge11);

            halfEdgeFaces.push_back(face012);
            halfEdgeFaces.push_back(face310);
            halfEdgeFaces.push_back(face213);
            halfEdgeFaces.push_back(face302);

            AssertEdgeValidity();
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

            math::vec3* a = &vertices.at(0);
            math::vec3* b = &vertices.at(1);
            math::vec3* c = &vertices.at(2);
            math::vec3* d = &vertices.at(3);
            math::vec3* e = &vertices.at(4);
            math::vec3* f = &vertices.at(5);
            math::vec3* g = &vertices.at(6);
            math::vec3* h = &vertices.at(7);

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
                assert(edge->edgePositionPtr);

                
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
                faceEdges.push_back(new HalfEdgeEdge(vert));
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
            for (int i = 1; i < mesh.vertices.size(); ++i)
            {
                if (mesh.vertices.at(i).x < outer[0]) outer[0] = i;
                else if (mesh.vertices.at(i).x > outer[1]) outer[1] = i;
                if (mesh.vertices.at(i).y < outer[2]) outer[2] = i;
                else if (mesh.vertices.at(i).y > outer[3]) outer[3] = i;
                if (mesh.vertices.at(i).z < outer[4]) outer[4] = i;
                else if (mesh.vertices.at(i).z > outer[5]) outer[5] = i;
            }

            // Check if it is a plane, min == max

            float largestDistance = 0;
            int index0;
            int index1;
            // Find the largest distance between two vertices in the box
            for (int i = 0; i < 6; ++i)
            {
                for (int j = i; j < 6; ++j)
                {
                    float dist = distance(mesh.vertices[i], mesh.vertices[j]);
                    if (dist > largestDistance)
                    {
                        index0 = i;
                        index1 = j;
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

            float largestDistance = 0;
            // Save the index which is furthest from the line between the mesh vertices at lineStartIndex and lineEndIndex
            size_type index = mesh.vertices.size();
            for (int i = 0; i < mesh.vertices.size(); ++i)
            {
                if (i == lineStartIndex || i == lineEndIndex) continue;
                math::vec3 toLineOrigin = mesh.vertices[lineStartIndex] - mesh.vertices[i];
                // Get distance over line
                float distOnLine = math::dot(toLineOrigin, linedir);
                // closest point on the line to vert i
                math::vec3 closestPoint = mesh.vertices[lineStartIndex] + (linedir * distOnLine);
                math::vec3 vecToLine = closestPoint - mesh.vertices[i];
                // Distance to line squared (Do not get actual distance with sqrt, since sqrt is a heavy computation)
                float distToLineSq = (vecToLine.x * vecToLine.x) + (vecToLine.y + vecToLine.y) + (vecToLine.z * vecToLine.z);
                if (distToLineSq > largestDistance)
                {
                    largestDistance = distToLineSq;
                    index = i;
                }
            }
            return index;
        }

        /**@brief Function to find the index in a mesh which is closest to the plane between plane indices
         * @param mesh - The mesh with the vertices
         * @param planeIndex0 - First plane index
         * @param planeIndex1 - Second plane index
         * @param planeIndex2 - Third plane index
         * @param normal - The normal of the plane index
         * @return The index of the vertex with the largest distance from the plane, mesh.vertices.size() is returned if a such a point cannot be found
         */
        size_type convexHullFindIndexClosestToPlane(core::mesh& mesh, size_type planeIndex0, size_type planeIndex1, size_type planeIndex2, math::vec3 normal)
        {
            float largestDistance = 0;
            // Save the index which is furthest from the plane between the mesh vertices at the plane indices
            int index = mesh.vertices.size();
            for (int i = 0; i < mesh.vertices.size(); ++i)
            {
                if (i == planeIndex0 || i == planeIndex1 || i == planeIndex2) continue;
                float dist = math::abs(math::dot(normal, mesh.vertices.at(i) - mesh.vertices.at(planeIndex0)));
                if (dist > largestDistance)
                {
                    largestDistance = dist;
                    index = i;
                }
            }
            return index;
        }

        std::vector<math::vec3> vertices;
        std::vector<HalfEdgeFace*> halfEdgeFaces;

        //feature id container
	};
}


