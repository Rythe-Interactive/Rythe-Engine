#include <physics/halfedgeedge.hpp>
#include <physics/halfedgeface.hpp>

namespace legion::physics
{
    void HalfEdgeEdge::setNextAndPrevEdge(HalfEdgeEdge* newPrevEdge, HalfEdgeEdge* newNextEdge)
    {
        nextEdge = newNextEdge;
        prevEdge = newPrevEdge;
    }

    void HalfEdgeEdge::setNext(HalfEdgeEdge* newNextEdge)
    {
        nextEdge = newNextEdge;
        newNextEdge->prevEdge = this;

    }

    void HalfEdgeEdge::setPrev(HalfEdgeEdge* newPrevEdge)
    {
        prevEdge = newPrevEdge;
        newPrevEdge->nextEdge = this;
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

    void HalfEdgeEdge::suicidalMergeWithPairing(math::mat4 transform)
    {
        //HalfEdgeFace* otherFace = pairingEdge->face;
        //face->startEdge = this->nextEdge;
        //pairingEdge->prevEdge->setNext(this->nextEdge);
        //prevEdge->setNext(pairingEdge->nextEdge);

        ////re-initialize face so that we take the new edges into account
        //face->initializeFace();
       
        ////pairingEdge->prevEdge->DEBUG_drawEdge(transform, math::colors::red, FLT_MAX, 5.0f);
        ////this->nextEdge->DEBUG_drawEdge(transform, math::colors::blue, FLT_MAX, 5.0f);

        ////prevEdge->DEBUG_drawEdge(transform, math::colors::darkgrey, FLT_MAX, 5.0f);
        ////pairingEdge->nextEdge->DEBUG_drawEdge(transform, math::colors::magenta, FLT_MAX, 5.0f);

        ////identify 

        ////nulify otherFace so its edges wont get deleted


        

        //----//
        HalfEdgeFace* mergeFace = pairingEdge->face;
        HalfEdgeEdge* prevFromCurrent = this->prevEdge->pairingEdge->face == mergeFace ? this->nextEdge : this->prevEdge;

        //next-pairing->next
        HalfEdgeEdge* prevFromCurrentConnection = prevFromCurrent->nextEdge->pairingEdge->nextEdge;
        prevFromCurrent->setNext(prevFromCurrentConnection);

        HalfEdgeEdge* nextFromCurrent = this->nextEdge->pairingEdge->face == mergeFace ? this->prevEdge : this->nextEdge;
        HalfEdgeEdge* nextFromCurrentConnection = nextFromCurrent->prevEdge->pairingEdge->prevEdge;

        nextFromCurrent->setPrev(nextFromCurrentConnection);

        prevFromCurrent->DEBUG_drawEdge(transform, math::colors::red, FLT_MAX, 5.0f);
        prevFromCurrentConnection->DEBUG_drawEdge(transform, math::colors::blue, FLT_MAX, 5.0f);

        nextFromCurrent->DEBUG_drawEdge(transform, math::colors::darkgrey, FLT_MAX, 5.0f);
        nextFromCurrentConnection->DEBUG_drawEdge(transform, math::colors::magenta, FLT_MAX, 5.0f);

        face->startEdge = prevFromCurrent;
        face->initializeFace();

        if (prevEdge->pairingEdge->face == mergeFace)
        {
            delete prevEdge->pairingEdge;
            delete prevEdge;
        }

        if (nextEdge->pairingEdge->face == mergeFace)
        {
            delete nextEdge->pairingEdge;
            delete nextEdge;
        }


        mergeFace->startEdge = nullptr;
        delete mergeFace;

        delete pairingEdge;
        delete this;

    }
}

