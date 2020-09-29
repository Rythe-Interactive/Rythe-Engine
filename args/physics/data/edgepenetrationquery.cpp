#include <physics/data/edgepenetrationquery.h>


namespace args::physics
{
    EdgePenetrationQuery::EdgePenetrationQuery(HalfEdgeEdge* pRefEdge, HalfEdgeEdge* pIncEdge,
        math::vec3& pFaceCentroid, math::vec3& pNormal, float& pPenetration, bool pIsARef) :
        PenetrationQuery(pFaceCentroid,pNormal,pPenetration,pIsARef) {}


    void EdgePenetrationQuery::populateContactList(physics_manifold& manifold)
    {
        log::debug(" EdgePenetrationQuery::populateContactList");
    }

}
