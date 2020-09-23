#include <physics/convexcollider.hpp>
#include <physics/physics_manifold.hpp>
#include <physics/HalfEdgeFace.h>
#include <physics/physicsimport.h>


namespace args::physics
{
    ConvexCollider::ConvexCollider() 
    {

    }

    ConvexCollider::~ConvexCollider()
    {
        for (auto face : halfEdgeFaces)
        {
            delete face;
        }
    }

    void ConvexCollider::CheckCollision(const std::shared_ptr<PhysicsCollider> physicsComponent, physics_manifold* manifold)
    {
        physicsComponent->CheckCollisionWith(this, manifold);
    }

    void ConvexCollider::CheckCollisionWith(ConvexCollider* convexCollider, physics_manifold* manifold)
    {

    }

    void ConvexCollider::UpdateTightAABB(math::mat4 transform)
    {
    }

    void ConvexCollider::ConstructConvexHullWithMesh()
    {
    }

    void ConvexCollider::ConstructBoxWithMesh()
    {

    }

    void ConvexCollider::CreateBox(const cube_collider_params& cubeParams)
    {
        //initialize box vertices
        float halfWidth = cubeParams.width / 2;
        float halfBreath = cubeParams.breadth / 2;
        float halfHeight = cubeParams.height / 2;

        //a = minVertexPlusBreadth 
        //b = minVertexPlusWidthPlusBreadth 
        //c = minVertex
        //d = minVertexPlusWidth

        //e = maxVertexMinusWidth
        //f = maxVertex
        //g = maxVertexMinusWidthMinusBreadth
        //h = maxVertexMinusBreadth

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
        math::vec3* a = &vertices.back();

        vertices.push_back(minVertexPlusWidthPlusBreadth);
        math::vec3* b = &vertices.back();

        vertices.push_back(minVertex);
        math::vec3* c = &vertices.back();

        vertices.push_back(minVertexPlusWidth);
        math::vec3* d = &vertices.back();

        vertices.push_back(maxVertexMinusWidth);
        math::vec3* e = &vertices.back();

        vertices.push_back(maxVertex);
        math::vec3* f = &vertices.back();

        vertices.push_back(maxVertexMinusWidthMinusBreadth);
        math::vec3* g = &vertices.back();

        vertices.push_back(maxVertexMinusBreadth);
        math::vec3* h = &vertices.back();

        for (auto& vertex : vertices)
        {
            vertex += cubeParams.offset;
        }

        //note: each edge carries adjacency information. (for example, an edge 'eg' must know its edge pair 'ge').
        //This is why each edge must be declared explicitly.

        //[1] create face eghf

            HalfEdgeEdge* eg = new HalfEdgeEdge(e);
            HalfEdgeEdge* gh = new HalfEdgeEdge(g);
            HalfEdgeEdge* hf = new HalfEdgeEdge(h);
            HalfEdgeEdge* fe = new HalfEdgeEdge(f);

            eg->setNextAndPrevEdge(fe,gh);
            gh->setNextAndPrevEdge(eg, hf);
            hf->setNextAndPrevEdge(gh, fe);
            fe->setNextAndPrevEdge(hf, eg);

           HalfEdgeFace* eghf = new HalfEdgeFace(eg, math::vec3(0,1,0));
           halfEdgeFaces.push_back(eghf);

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

        //[4] create face efba

            HalfEdgeEdge* ef = new HalfEdgeEdge(e);
            HalfEdgeEdge* fb = new HalfEdgeEdge(f);
            HalfEdgeEdge* ba = new HalfEdgeEdge(b);
            HalfEdgeEdge* ae = new HalfEdgeEdge(a);

            ef->setNextAndPrevEdge(ae,fb);
            fb->setNextAndPrevEdge(ef,ba);
            ba->setNextAndPrevEdge(fb,ae);
            ae->setNextAndPrevEdge(ba,ef);

            HalfEdgeFace* efba = new HalfEdgeFace(ef, math::vec3(0, 0, 1));
            halfEdgeFaces.push_back(efba);

        //[5] create face geac

            HalfEdgeEdge* ge = new HalfEdgeEdge(g);
            HalfEdgeEdge* ea = new HalfEdgeEdge(e);
            HalfEdgeEdge* ac = new HalfEdgeEdge(a);
            HalfEdgeEdge* cg = new HalfEdgeEdge(c);

            ge->setNextAndPrevEdge(cg,ea);
            ea->setNextAndPrevEdge(ge,ac);
            ac->setNextAndPrevEdge(ea,cg);
            cg->setNextAndPrevEdge(ac,ge);

            HalfEdgeFace* geac = new HalfEdgeFace(ge, math::vec3(-1, 0, 0));
            halfEdgeFaces.push_back(geac);

        //[6] create face abdc

            HalfEdgeEdge* ab = new HalfEdgeEdge(a);
            HalfEdgeEdge* bd = new HalfEdgeEdge(b);
            HalfEdgeEdge* dc = new HalfEdgeEdge(d);
            HalfEdgeEdge* ca = new HalfEdgeEdge(c);

            ab->setNextAndPrevEdge(ca,bd);
            bd->setNextAndPrevEdge(ab,dc);
            dc->setNextAndPrevEdge(bd,ca);
            ca->setNextAndPrevEdge(dc,ab);

            HalfEdgeFace* abdc = new HalfEdgeFace(ab, math::vec3(0, -1, 0));
            halfEdgeFaces.push_back(abdc);



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

        //check if halfEdge data structure was initialized correctly. this will be commented when I know it always works
        
        auto assertFunc = [](HalfEdgeEdge* edge)
        {
            assert(edge->nextEdge);
            assert(edge->prevEdge);
            assert(edge->pairingEdge);
            assert(edge->edgePositionPtr);
        };

        for( auto& face : halfEdgeFaces)
        {
            face->forEachEdge(assertFunc);
        }
        

    }

    HalfEdgeFace* ConvexCollider::instantiateMeshFace(const std::vector<math::vec3*>& vertices, const math::vec3& faceNormal)
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




}

