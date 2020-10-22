
#include <physics/data/edgepenetrationquery.h>
#include <physics/physics_statics.hpp>



namespace args::physics
{
    EdgePenetrationQuery::EdgePenetrationQuery(HalfEdgeEdge* pRefEdge, HalfEdgeEdge* pIncEdge,
        math::vec3& pFaceCentroid, math::vec3& pNormal, float& pPenetration, bool pIsARef) :
        PenetrationQuery(pFaceCentroid,pNormal,pPenetration,pIsARef),refEdge(pRefEdge),incEdge(pIncEdge) {}


    void EdgePenetrationQuery::populateContactList(physics_manifold& manifold, math::mat4& refTransform, math::mat4 incTransform)
    {
        //------------------- The contact points between 2 edges are the closest points between the 2 edges --------------------//
        log::debug("EdgePenetrationQuery::populateContactList");

        math::vec3 p1 = refTransform * math::vec4(*refEdge->edgePositionPtr, 1);
        math::vec3 p2 = refTransform * math::vec4(*refEdge->nextEdge->edgePositionPtr, 1);

        math::vec3 p3 = incTransform * math::vec4(*incEdge->edgePositionPtr, 1);
        math::vec3 p4 = incTransform * math::vec4(*incEdge->nextEdge->edgePositionPtr, 1);

        math::vec3 refContactPoint;
        math::vec3 incContactPoint;

        PhysicsStatics::FindClosestPointsToLineSegment(p1, p2, p3, p4, refContactPoint, incContactPoint);

        physics_contact contact;

        contact.IncWorldContact = incContactPoint;
        contact.RefWorldContact = refContactPoint;

        manifold.contacts.push_back(contact);

    }

}
