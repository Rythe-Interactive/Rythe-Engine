#pragma once

#include <limits>
#include <core/core.hpp>
#include <physics/diviner/colliders/physicscollider.hpp>
#include <physics/diviner/cube_collider_params.hpp>
#include <physics/diviner/halfedgeedge.hpp>
#include <physics/diviner/halfedgeface.hpp>
#include <physics/diviner/data/convex_convergence_identifier.hpp>
#include <physics/diviner/data/physics_manifold.hpp>
#include <rendering/debugrendering.hpp>

namespace legion::physics
{
    class ConvexCollider : public PhysicsCollider
    {
    public:
        bool debug = false;
        int step = 0;
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
        void AddConvergenceIdentifier(const physics_contact& contact) override;

        void CheckCollision(PhysicsCollider* physicsCollider, physics_manifold& manifold) override
        {
            physicsCollider->CheckCollisionWith(this, manifold);
        }

        /** @brief Given a ConvexCollider and a physics_manifold, uses the seperating axis test for each HalfEdgeFace and edge
        * of both ConvexColliders
        */
        void CheckCollisionWith(ConvexCollider* convexCollider, physics_manifold& manifold) override;

        void PopulateContactPoints(PhysicsCollider* physicsCollider, physics_manifold& manifold) override
        {
            physicsCollider->PopulateContactPointsWith(this, manifold);
        }

        void PopulateContactPointsWith(ConvexCollider* convexCollider, physics_manifold& manifold) override;

        void UpdateTransformedTightBoundingVolume(const math::mat4& transform) override
        {
            UpdateTightAABB(transform);
        }

        /**@brief Given the current transform of the entity, creates a tight AABB of the collider;
        */
        void UpdateTightAABB(const math::mat4& transform);
       
        virtual void DrawColliderRepresentation(const math::mat4& transform, math::color usedColor, float width,float time,bool ignoreDepth = false) override;
        

        /**@brief Does one step of the convex hull generation
         * Just for debug purposes
         */
        void doStep(legion::core::mesh& mesh)
        {
           
            ConstructConvexHullWithMesh(mesh);
            ++step;
        }

        void ConstructConvexHullWithVertices( std::vector<math::vec3>& vertices,math::vec3 spacingAmount = math::vec3())
        {
            //many of the functions for convex hull are coupled to mesh,
            //for now we create a mesh so that we can pass it into the function
            core::mesh mesh;
            mesh.vertices = std::move(vertices);
            ConstructConvexHullWithMesh(mesh,spacingAmount);
        }

        //TODO(algorythmix,jelled1st) This desperately needs cleanup
        //TODO(cont.) investigate unused variables! (projected)
        //LIKE A LOT OF CLEANUP

        void  ConstructConvexHullWithMesh(mesh& mesh, math::vec3 spacingAmount = math::vec3(), bool shouldDebug = false);
       
        /**@brief Constructs a box-shaped convex hull that encompasses the given mesh.
        */
        void ConstructBoxWithMesh()
        {

        }

        void CalculateLocalColliderCentroid()
        {
            localColliderCentroid = math::vec3();

            for (auto& vertex : vertices)
            {
                localColliderCentroid += vertex;
            }
            
            localColliderCentroid /= vertices.size();
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

            for (auto& vertex : vertices)
            {
                vertex += cubeParams.offset;
            }

            math::vec3 a = vertices.at(0);
            math::vec3 b = vertices.at(1);
            math::vec3 c = vertices.at(2);
            math::vec3 d = vertices.at(3);
            math::vec3 e = vertices.at(4);
            math::vec3 f = vertices.at(5);
            math::vec3 g = vertices.at(6);
            math::vec3 h = vertices.at(7);

            CalculateLocalColliderCentroid();

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

            //edge pairings have been set, we can calculate edge directions now
            for (HalfEdgeFace* face : halfEdgeFaces)
            {
                auto calculateDirection = [](HalfEdgeEdge* edge) {edge->calculateRobustEdgeDirection(); };
                face->forEachEdge(calculateDirection);
            }

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
            CalculateLocalColliderCentroid();
        }

        std::vector<HalfEdgeFace*>& GetHalfEdgeFaces() override
        {
            return halfEdgeFaces;
        }

        std::vector<math::vec3>& GetVertices() 
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

        /**@brief populates the vertices vector with the 
         * edge positions of the each face in halfEdgeFace
         */
        void populateVertexListWithHalfEdges();

    private:


        std::vector<math::vec3> vertices;

        std::vector<HalfEdgeFace*> halfEdgeFaces;
    };
}


