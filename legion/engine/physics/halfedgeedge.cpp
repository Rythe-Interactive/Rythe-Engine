#include <physics/halfedgeedge.hpp>
#include <physics/halfedgeface.hpp>

namespace legion::physics
{
    void HalfEdgeEdge::setNextAndPrevEdge(HalfEdgeEdge* newPrevEdge, HalfEdgeEdge* newNextEdge)
    {
        nextEdge = newNextEdge;
        prevEdge = newPrevEdge;
    }

    void HalfEdgeEdge::setPairingEdge(HalfEdgeEdge* edge)
    {
        pairingEdge = edge;
        edge->pairingEdge = this;
    }

    math::vec3 HalfEdgeEdge::getLocalNormal() const
    {
        return face->normal;
    }

    math::vec3 HalfEdgeEdge::getLocalEdgeDirection() const
    {
        return nextEdge->edgePosition - edgePosition;
    }

    bool HalfEdgeEdge::isVertexVisible(const math::vec3& vert)
    {
        float distanceToPlane =
            math::pointToPlane(vert, edgePosition, face->normal);

        return distanceToPlane > math::sqrt(math::epsilon<float>());
    }

    bool HalfEdgeEdge::isEdgeHorizonFromVertex(const math::vec3& vert)
    {
        return isVertexVisible(vert) && !pairingEdge->isVertexVisible(vert);
    }

    void HalfEdgeEdge::DEBUG_drawEdge(const math::mat4& transform, const math::color& debugColor, float time, float width)
    {
        math::vec3 worldStart = transform * math::vec4(edgePosition, 1);
        math::vec3 worldEnd = transform * math::vec4(nextEdge->edgePosition, 1);

        debug::user_projectDrawLine(worldStart, worldEnd, debugColor, width, time, true);
    }

    void HalfEdgeEdge::DEBUG_drawInsetEdge(const math::vec3 spacing, const math::color& debugColor, float time, float width)
    {
        math::vec3 worldCentroid = face->centroid + spacing;

        math::vec3 worldStart = edgePosition + spacing;
        math::vec3 startDifference = (worldCentroid - worldStart) * 0.1f;

        math::vec3 worldEnd = nextEdge->edgePosition + spacing;
        math::vec3 endDifference = (worldCentroid - worldEnd) * 0.1f;


        debug::user_projectDrawLine(worldStart + startDifference, worldEnd + endDifference, debugColor, width, time, true);
    }
}

