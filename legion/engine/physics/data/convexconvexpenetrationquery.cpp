
#include <physics/data/convexconvexpenetrationquery.hpp>
#include <physics/physics_statics.hpp>
#include <physics/physics_contact.hpp>
#include <physics/data/contact_vertex.hpp>

namespace legion::physics
{
    ConvexConvexPenetrationQuery::ConvexConvexPenetrationQuery(HalfEdgeFace* pRefFace, HalfEdgeFace* pIncFace,
        math::vec3& pFaceCentroid, math::vec3& pNormal, float pPenetration, bool pIsARef)
        :  PenetrationQuery(pFaceCentroid,pNormal,pPenetration,pIsARef),refFace(pRefFace), incFace(pIncFace)
    {
        debugID = "ConvexConvexPenetrationQuery";
    }

    void ConvexConvexPenetrationQuery::populateContactList(physics_manifold& manifold, math::mat4& refTransform
        , math::mat4 incTransform)
    {
        bool shouldDebug = false;// manifold.DEBUG_checkID("floor", "problem");
        auto refCollider = isARef ? manifold.colliderA : manifold.colliderB;
        auto incCollider = isARef ? manifold.colliderB : manifold.colliderA;

        //penetration
        if (shouldDebug)
        {
            log::debug("//////ConvexConvexPenetrationQuery::populateContactList");
        }

        //
        float largestDotResult = std::numeric_limits<float>::lowest();

        for (auto face : incCollider->GetHalfEdgeFaces())
        {
            math::vec3 worldFaceNormal = incTransform * math::vec4(face->normal, 0);
            float currentDotResult = math::dot(-normal, worldFaceNormal);

            if (currentDotResult > largestDotResult)
            {
                largestDotResult = currentDotResult;
                incFace = face;
            }
        }


        //------------------------------- get all world vertex positions in incFace -------------------------------------------------//
        std::vector<ContactVertex> outputContactPoints;

        bool facePartiallyBelowPlane = false;

        auto sendToInitialOutput = [&outputContactPoints,&incTransform](HalfEdgeEdge* edge)
        {
            math::vec3 localVertexPosition = edge->edgePosition;
            math::vec3 worldVertex = incTransform * math::vec4(localVertexPosition, 1);

            outputContactPoints.push_back(ContactVertex(worldVertex,edge->label));
        };

        incFace->forEachEdge(sendToInitialOutput);


        if (shouldDebug)
        {
            log::debug("initial input size {} ", outputContactPoints.size());
        }
        //------------------------------- clip vertices with faces that are the neighbors of refFace  ---------------------------------//
        auto clipNeigboringFaceWithOutput = [&refTransform,&outputContactPoints,shouldDebug](HalfEdgeEdge* edge)
        {
            HalfEdgeFace* neighborFace = edge->pairingEdge->face;
            math::vec3 planePosition = refTransform * math::vec4(neighborFace->centroid, 1);
            math::vec3 planeNormal = refTransform * math::vec4(neighborFace->normal, 0);

            auto inputContactList = outputContactPoints;
            outputContactPoints.clear();

            if (shouldDebug)
            {
                log::debug("splitting with plane position {} and normal {} ",
                    math::to_string(planePosition), math::to_string(planeNormal));
            }

            PhysicsStatics::SutherlandHodgmanFaceClip(planeNormal, planePosition, inputContactList, outputContactPoints,edge);

            if (shouldDebug)
            {
                log::debug("outputContactPoints {} ",
                    outputContactPoints.size());
            }

        };

        refFace->forEachEdge(clipNeigboringFaceWithOutput);

      

        if (shouldDebug)
        {
            log::debug("->  iterating through contacts");
        }

        for (const auto& incidentContact : outputContactPoints)
        {
            float distanceToCollisionPlane = PhysicsStatics::PointDistanceToPlane(normal, faceCentroid, incidentContact.position);

            if (distanceToCollisionPlane < constants::contactOffset)
            {
                /*if (shouldDebug)
                {
                    log::debug("=pushed contact");
                }*/

                math::vec3 referenceContact = incidentContact.position - normal * distanceToCollisionPlane;

                physics_contact contact;
                contact.refCollider = refCollider;
                contact.IncWorldContact = incidentContact.position;
                contact.RefWorldContact = referenceContact;
                contact.label = incidentContact.label;

                refCollider->AttemptFindAndCopyConverganceID(contact);

                manifold.contacts.push_back(contact);
             
            }


        }
        //log::debug("//////ConvexConvexPenetrationQuery::populateContactList");
    }


}
