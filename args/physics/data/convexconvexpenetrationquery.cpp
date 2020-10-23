
#include <physics/data/convexconvexpenetrationquery.hpp>
#include <physics/physics_statics.hpp>
#include <physics/physics_contact.h>

namespace args::physics
{
    ConvexConvexPenetrationQuery::ConvexConvexPenetrationQuery(HalfEdgeFace* pRefFace, HalfEdgeFace* pIncFace,
        math::vec3& pFaceCentroid, math::vec3& pNormal, float pPenetration, bool pIsARef)
        :  PenetrationQuery(pFaceCentroid,pNormal,pPenetration,pIsARef),refFace(pRefFace), incFace(pIncFace)
    {

    }

    void ConvexConvexPenetrationQuery::populateContactList(physics_manifold& manifold, math::mat4& refTransform, math::mat4 incTransform)
    {
        //------------------------------- get all world vertex positions in incFace -------------------------------------------------//
        std::vector<math::vec3> outputContactPoints;

        auto sendToInitialOutput = [&outputContactPoints,&incTransform](HalfEdgeEdge* edge)
        {
            math::vec3 localVertexPosition = *edge->edgePositionPtr;
            math::vec3 worldVertexPosition = incTransform * math::vec4(localVertexPosition, 1);
            outputContactPoints.push_back(incTransform * math::vec4(localVertexPosition, 1));
        };

        incFace->forEachEdge(sendToInitialOutput);

        //------------------------------- clip vertices with faces that are the neighbors of refFace  ---------------------------------//
        auto clipNeigboringFaceWithOutput = [&refTransform,&outputContactPoints](HalfEdgeEdge* edge)
        {
            HalfEdgeFace* neighborFace = edge->pairingEdge->face;
            math::vec3 planePosition = refTransform * math::vec4(neighborFace->centroid, 1);
            math::vec3 planeNormal = refTransform * math::vec4(neighborFace->normal, 0);

            auto inputContactList = outputContactPoints;
            outputContactPoints.clear();
        
            PhysicsStatics::SutherlandHodgmanFaceClip(planeNormal, planePosition, inputContactList, outputContactPoints);
        };

        refFace->forEachEdge(clipNeigboringFaceWithOutput);

        //-------- get the contact points of the ref polyhedron by projecting the incident contacts to the collision plane ---------//

        for (const auto& incidentContact : outputContactPoints)
        {
            float distanceToCollisionPlane = PhysicsStatics::PointDistanceToPlane(normal, faceCentroid, incidentContact);

            if (distanceToCollisionPlane < 0.05f)
            {
                math::vec3 referenceContact = incidentContact - normal * distanceToCollisionPlane;

                physics_contact contact;

                contact.IncWorldContact = incidentContact;
                contact.RefWorldContact = referenceContact;

                //log::debug("incidentContact {} ", math::to_string(incidentContact));

                manifold.contacts.push_back(contact);
             

            }
        }

    }


}
