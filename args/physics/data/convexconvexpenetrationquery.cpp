
#include <physics/data/convexconvexpenetrationquery.h>

namespace args::physics
{
    ConvexConvexPenetrationQuery::ConvexConvexPenetrationQuery(HalfEdgeFace* pRefFace, HalfEdgeFace* pIncFace,
        math::vec3& pFaceCentroid, math::vec3& pNormal, float& pPenetration, bool pIsARef)
        : PenetrationQuery(pFaceCentroid,pNormal,penetration,pIsARef),refFace(pRefFace),incFace(pIncFace)
    {

    }

    void ConvexConvexPenetrationQuery::populateContactList(physics_manifold& manifold)
    {
        log::debug("ConvexConvexPenetrationQuery::populateContactList");
    }


}
