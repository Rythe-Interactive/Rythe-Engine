
#include <physics/data/edgepenetrationquery.h>
#include <physics/physics_statics.hpp>
#include <physics/systems/physicssystem.hpp>


namespace args::physics
{
    EdgePenetrationQuery::EdgePenetrationQuery(HalfEdgeEdge* pRefEdge, HalfEdgeEdge* pIncEdge,
        math::vec3& pFaceCentroid, math::vec3& pNormal, float& pPenetration, bool pIsARef) :
        PenetrationQuery(pFaceCentroid,pNormal,pPenetration,pIsARef),refEdge(pRefEdge),incEdge(pIncEdge) {}


    void EdgePenetrationQuery::populateContactList(physics_manifold& manifold, math::mat4& refTransform, math::mat4 incTransform)
    {
        if (!refEdge) { return; }
        log::debug(" EdgePenetrationQuery::populateContactList");

        assert(refEdge);
        assert(incEdge);

        math::vec3 p1 = refTransform * math::vec4(*refEdge->edgePositionPtr, 1);
        math::vec3 p2 = refTransform * math::vec4(*refEdge->nextEdge->edgePositionPtr, 1);

        math::vec3 p3 = incTransform * math::vec4(*incEdge->edgePositionPtr, 1);
        math::vec3 p4 = incTransform * math::vec4(*incEdge->nextEdge->edgePositionPtr, 1);

        math::vec3 refContactPoint;
        math::vec3 incContactPoint;

       

        PhysicsStatics::FindClosestPointsToLineSegment(p1, p2, p3, p4, refContactPoint, incContactPoint);

        physics_contact contact;

        contact.incTransform = incTransform;
        contact.refTransform = refTransform;

        contact.worldContactInc = incContactPoint;
        contact.worldContactRef = refContactPoint;

        manifold.contacts.push_back(contact);
        PhysicsSystem::contactPoints.push_back(contact);


        //physics_contact fakecontact1;

        //fakecontact1.worldContactInc = p1;
        //fakecontact1.worldContactRef = p2;

        //physics_contact fakecontact2;

        //fakecontact2.worldContactInc = p3;
        //fakecontact2.worldContactRef = p4;

        //PhysicsSystem::contactPoints.push_back(fakecontact1);
        //PhysicsSystem::contactPoints.push_back(fakecontact2);

    }

}
