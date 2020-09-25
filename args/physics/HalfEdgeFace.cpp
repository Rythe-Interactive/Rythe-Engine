#include <physics/HalfEdgeFace.h>
#include <physics/HalfEdgeEdge.h>

namespace args::physics
{
    HalfEdgeFace::HalfEdgeFace(HalfEdgeEdge* newStartEdge, math::vec3 newNormal) : startEdge{ newStartEdge }, normal{ newNormal }
    {
        auto setToFaceFunc = [this](HalfEdgeEdge* edge)
        {
            edge->face = this;
        };

        forEachEdge(setToFaceFunc);

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
       
    }


    void HalfEdgeFace::forEachEdge(args::core::delegate< void(HalfEdgeEdge*)> functionToExecute)
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
