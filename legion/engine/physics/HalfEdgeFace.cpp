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
            math::vec3 pos = edge->edgePosition;
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

    int HalfEdgeFace::edgeCount() const
    {
        int count = 0;
        HalfEdgeEdge* current = startEdge;
        do
        {
            ++count;
            current = current->nextEdge;
        } while (current != startEdge);

        return count;
    }

    HalfEdgeEdge* HalfEdgeFace::getEdgeN(int n)
    {
        HalfEdgeEdge* edge = startEdge;
        for (; n > 0; --n)
        {
            edge = edge->nextEdge;
        }
        return edge;
    }

    void HalfEdgeFace::inverse()
    {
        HalfEdgeEdge* start = startEdge;
        HalfEdgeEdge* current = startEdge;
        if (start->nextEdge == start)
        {
            return;
        }
        do
        {
            HalfEdgeEdge* prev = current->prevEdge;
            current->prevEdge = current->nextEdge;
            current->nextEdge = prev;

            // Current should go the edge that was previously the next
            current = current->prevEdge;
        } while (current != start);

        normal = -normal; // Inverse the normal
    }

    bool HalfEdgeFace::testConvexity(const HalfEdgeFace& other) const
    {
        math::vec3 difference = startEdge->edgePosition - other.centroid;
        float scaledAngle = math::dot(difference, normal);

        // if the scaledAngle is smaller or equal to 0, it is not convex
        if (scaledAngle <= 0)
        {
            return false;
        }

        return true;
    }

    bool HalfEdgeFace::makeNormalsConvexWithFace(HalfEdgeFace& other)
    {
        math::vec3 difference = startEdge->edgePosition - other.centroid;
        float scaledAngle = math::dot(difference, normal);

        // if the scaledAngle is smaller or equal to 0, it is not convex
        if (scaledAngle <= 0)
        {
            inverse();
            return true;
        }
        return false;
    }

	bool HalfEdgeFace::testConvexity(const HalfEdgeFace& first, const HalfEdgeFace& second)
	{
		return first.testConvexity(second) && second.testConvexity(first);
	}

    bool HalfEdgeFace::makeNormalsConvexWithFace(HalfEdgeFace& first, HalfEdgeFace& second)
    {
        bool inversedNormal = first.makeNormalsConvexWithFace(second);
        inversedNormal |= second.makeNormalsConvexWithFace(first);
        return inversedNormal;
    }

    HalfEdgeFace::~HalfEdgeFace()
    {
        auto deleteFunc = [](HalfEdgeEdge* edge) { delete edge; };

        forEachEdge(deleteFunc);

    }




}
