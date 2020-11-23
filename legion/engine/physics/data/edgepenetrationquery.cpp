
#include <physics/data/edgepenetrationquery.hpp>
#include <physics/physics_statics.hpp>



namespace legion::physics
{
    EdgePenetrationQuery::EdgePenetrationQuery(HalfEdgeEdge* pRefEdge, HalfEdgeEdge* pIncEdge,
        math::vec3& pFaceCentroid, math::vec3& pNormal, float& pPenetration, bool pIsARef) :
        PenetrationQuery(pFaceCentroid,pNormal,pPenetration,pIsARef),refEdge(pRefEdge),incEdge(pIncEdge) {}


    void EdgePenetrationQuery::populateContactList(physics_manifold& manifold,
        math::mat4& refTransform, math::mat4 incTransform, std::shared_ptr<PhysicsCollider> refCollider)
    {
        //------------------- The contact points between 2 edges are the closest points between the 2 edges --------------------//
        //log::debug("EdgePenetrationQuery::populateContactList");

        math::vec3 p1 = refTransform * math::vec4(*refEdge->edgePositionPtr, 1);
        math::vec3 p2 = refTransform * math::vec4(*refEdge->nextEdge->edgePositionPtr, 1);

        math::vec3 p3 = incTransform * math::vec4(*incEdge->edgePositionPtr, 1);
        math::vec3 p4 = incTransform * math::vec4(*incEdge->nextEdge->edgePositionPtr, 1);

        math::vec3 refContactPoint;
        math::vec3 incContactPoint;

        PhysicsStatics::FindClosestPointsToLineSegment(p1, p2, p3, p4, refContactPoint, incContactPoint);

        physics_contact contact;

        auto refLabel = refEdge->label;
        auto incLabel = incEdge->label;

        contact.label = EdgeLabel(std::make_pair(refLabel.firstEdge.first, refLabel.firstEdge.second),
            std::make_pair(incLabel.nextEdge.first, incLabel.nextEdge.second));

        contact.refCollider = refCollider;
        contact.IncWorldContact = incContactPoint;
        contact.RefWorldContact = refContactPoint;

        refCollider->AttemptFindAndCopyConverganceID(contact);

        manifold.contacts.push_back(contact);

    }

}
