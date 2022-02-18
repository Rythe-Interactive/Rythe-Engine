#include <physics/diviner/halfedgeface.hpp>
#include <rendering/debugrendering.hpp>
#include <physics/diviner/physics_statics.hpp>

namespace legion::physics
{
    HalfEdgeFace::HalfEdgeFace(HalfEdgeEdge* newStartEdge, math::vec3 newNormal) : startEdge{ newStartEdge }, normal{ newNormal }
    {
        initializeFace();
    }

    void HalfEdgeFace::initializeFace()
    {
        static int faceCount = 0;

        math::vec3 faceCenter{ 0.0f };
        int edgeCount = 0;

        auto calculateFaceCentroid = [&faceCenter, &edgeCount](HalfEdgeEdge* edge)
        {
            faceCenter += edge->edgePosition;
            edgeCount++;
        };
        forEachEdge(calculateFaceCentroid);

        centroid = faceCenter / static_cast<float>(edgeCount);

        int currentEdgeId = 0;

        auto initializeEdgeToFaceFunc = [this, &currentEdgeId, edgeCount](HalfEdgeEdge* edge)
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

    float HalfEdgeFace::calculateFaceExtents()
    {
        //get vector of vertices of face
        std::vector<math::vec3> vertices;
        vertices.reserve(6);

        auto collectVertices = [&vertices](HalfEdgeEdge* edge) {vertices.push_back(edge->edgePosition); };
        forEachEdge(collectVertices);

        //get tangents of normals
        math::vec3 forward = math::normalize(centroid - startEdge->edgePosition);
        math::vec3 right = math::cross(normal, forward);

        math::vec3 maxForward, minForward, maxRight, minRight;

        //get support point for the tangents and inverse tangents of this face 
        PhysicsStatics::GetSupportPoint(vertices, forward, maxForward);
        PhysicsStatics::GetSupportPoint(vertices, -forward, minForward);

        PhysicsStatics::GetSupportPoint(vertices, right, maxRight);
        PhysicsStatics::GetSupportPoint(vertices, -right, minRight);

        float maxForwardLength = math::dot(maxForward - centroid, forward);
        float minForwardLength = math::dot(minForward - centroid, -forward);

        float maxRightLength = math::dot(maxRight - centroid, right);
        float minRightLength = math::dot(minRight - centroid, -right);

        return (maxForwardLength + minForwardLength) + (maxRightLength + minRightLength);
    }

    void HalfEdgeFace::forEachEdge(legion::core::delegate< void(HalfEdgeEdge*)> functionToExecute,
        legion::core::delegate <HalfEdgeEdge* (HalfEdgeEdge*)> getNextEdge )
    {
        HalfEdgeEdge* initialEdge = startEdge;
        HalfEdgeEdge* currentEdge = startEdge;

        if (!currentEdge) return;

        //the HalfEdgeEdge* 'startEdge' creates a ring buffer.
        //This means that initialEdge will eventually go back to "startEdge", ending the loop.
        do
        {
            HalfEdgeEdge* edgeToExecuteOn = currentEdge;
            currentEdge = getNextEdge(currentEdge);
            functionToExecute(edgeToExecuteOn);

        } while (initialEdge != currentEdge && getNextEdge(currentEdge) != nullptr);
    }

    void HalfEdgeFace::forEachEdgeReverse(legion::core::delegate<void(HalfEdgeEdge*)> functionToExecute)
    {
        auto getPrevEdges = [](HalfEdgeEdge* current) {return current->prevEdge; };

        forEachEdge(functionToExecute, getPrevEdges);
    }

    void HalfEdgeFace::inverse()
    {
        normal = -normal; // Inverse the normal

        //collect edges into std::vector
        std::vector<HalfEdgeEdge*> edges;

        auto collectEdges = [&edges](HalfEdgeEdge* edge)
        {
            edges.push_back(edge);
        };

        forEachEdgeReverse(collectEdges);

        for (size_type i = 0; i < edges.size(); i++)
        {
            size_type nextIndex = (i + 1) % edges.size();
            size_type prevIndex = (i == 0ull ? (edges.size() - 1ull) : (i - 1ull));

            HalfEdgeEdge* newNext = edges.at(prevIndex);
            HalfEdgeEdge* newPrev = edges.at(nextIndex);

            edges.at(i)->setNextAndPrevEdge(newNext, newPrev);
        }

        startEdge = edges.at(0);
    }

    void HalfEdgeFace::DEBUG_DrawFace(const math::mat4& transform,const math::color& debugColor,float time)
    {
        auto drawFunc = [&transform,debugColor,time](HalfEdgeEdge* edge)
        {
            edge->DEBUG_drawEdge(transform, debugColor, time);

        };

        math::vec3 worldStart = transform * math::vec4(centroid, 1);
        math::vec3 worldEnd = transform * math::vec4(centroid + normal * 0.1f, 1);

        forEachEdge(drawFunc);
    }

    void HalfEdgeFace::DEBUG_DirectionDrawFace(const math::mat4& transform, const math::color& debugColor, float time)
    {
        auto drawFunc = [&transform, debugColor, time](HalfEdgeEdge* edge)
        {
            edge->DEBUG_directionDrawEdge(transform, debugColor, time, 5.0f);
        };

        math::vec3 worldStart = transform * math::vec4(centroid, 1);
        math::vec3 worldEnd = transform * math::vec4(centroid + normal * 0.1f, 1);

        debug::drawLine(worldStart, worldEnd, math::colors::green, 3.0f, time, false);

        forEachEdge(drawFunc);
    }

    HalfEdgeFace::~HalfEdgeFace()
    {
        auto deleteFunc = [](HalfEdgeEdge* edge)
        {
            if (edge->pairingEdge && edge->pairingEdge && edge->pairingEdge->pairingEdge == edge)
            {
                edge->pairingEdge->pairingEdge = nullptr;
            }
            delete edge;
        };

        forEachEdge(deleteFunc);
    }
}
