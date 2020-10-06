#include <physics/data/edgepenetrationquery.h>


namespace args::physics
{
    EdgePenetrationQuery::EdgePenetrationQuery(HalfEdgeEdge* pRefEdge, HalfEdgeEdge* pIncEdge,
        math::vec3& pFaceCentroid, math::vec3& pNormal, float& pPenetration, bool pIsARef) :
        PenetrationQuery(pFaceCentroid,pNormal,pPenetration,pIsARef),refEdge(pRefEdge),incEdge(pIncEdge) {}


    void EdgePenetrationQuery::populateContactList(physics_manifold& manifold, math::mat4& refTransform, math::mat4 incTransform)
    {
        log::debug(" EdgePenetrationQuery::populateContactList");

        assert(refEdge);
        assert(incEdge);
    }

}
