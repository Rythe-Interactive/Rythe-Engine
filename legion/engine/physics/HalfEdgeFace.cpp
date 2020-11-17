#include <physics/halfedgeface.hpp>
#include <physics/halfedgeedge.hpp>

namespace legion::physics
{
    HalfEdgeFace::HalfEdgeFace(HalfEdgeEdge* newStartEdge, math::vec3 newNormal) : startEdge{ newStartEdge }, normal{ newNormal }
    {
        /*log::debug("HalfEdgeFace::HalfEdgeFace");*/
        static int faceCount = 0;

        math::vec3 faceCenter{ 0.0f };
        int edgeCount = 0;

        auto calculateFaceCentroid = [&faceCenter,&edgeCount](HalfEdgeEdge* edge)
        {
            math::vec3 pos = *(edge->edgePositionPtr);
            faceCenter += pos;
            edgeCount++;
        };
        forEachEdge(calculateFaceCentroid);

        centroid = faceCenter/static_cast<float>( edgeCount);

        int currentEdgeId = 0;

        auto initializeEdgeToFaceFunc = [this,&currentEdgeId,edgeCount](HalfEdgeEdge* edge)
        {
            edge->face = this;

            int nextID = currentEdgeId + 1 < edgeCount ? currentEdgeId + 1 : 0;

            EdgeLabel label
            (std::make_pair(faceCount, currentEdgeId), std::make_pair(faceCount, nextID));

            edge->label = std::move(label);

            currentEdgeId++;
        };

        forEachEdge(initializeEdgeToFaceFunc);

        faceCount++;
       
    }


    void HalfEdgeFace::forEachEdge(legion::core::delegate< void(HalfEdgeEdge*)> functionToExecute)
    {
        HalfEdgeEdge* initialEdge = startEdge;
        HalfEdgeEdge* currentEdge = startEdge;

        if (!currentEdge) { return; }

        //the HalfEdgeEdge* 'startEdge' creates a ring buffer.
        //This means that initialEdge will eventually go back to "startEdge", ending the loop.
        do
        {
            HalfEdgeEdge* edgeToExecuteOn = currentEdge;
            currentEdge = currentEdge->nextEdge;
            functionToExecute(edgeToExecuteOn);

        } while (initialEdge != currentEdge && currentEdge != nullptr);

    }

    HalfEdgeFace::~HalfEdgeFace()
    {
        auto deleteFunc = [](HalfEdgeEdge* edge) { delete edge; };

        forEachEdge(deleteFunc);

    }




}
