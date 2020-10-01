#pragma once
#include <physics/physicsimport.h>
#include <physics/halfedgeface.hpp>

namespace args::physics
{
	struct HalfEdgeEdge
	{
		HalfEdgeEdge* pairingEdge = nullptr;
		HalfEdgeEdge * nextEdge = nullptr;
		HalfEdgeEdge* prevEdge = nullptr;

		HalfEdgeFace* face = nullptr;

		math::vec3* edgePositionPtr;

        HalfEdgeEdge() = default;

		HalfEdgeEdge(math::vec3* newEdgePositionPtr) : edgePositionPtr{ newEdgePositionPtr }
		{

		}

		/**@brief sets 'nextEdge' and 'prevEdge' with the given HalfEdgeEdges
		*/
		void setNextAndPrevEdge(HalfEdgeEdge* newPrevEdge,HalfEdgeEdge* newNextEdge)
		{
			nextEdge = newNextEdge;
			prevEdge = newPrevEdge;
		}

		math::vec3 getLocalNormal() const
		{
			return face->normal;
		}

		/**@brief Gets the direction of the edge by getting the
		* vector starting from the current edge's position to the next edge
		*/
		math::vec3 getLocalEdgeDirection()  const
		{
			return *nextEdge->edgePositionPtr - *edgePositionPtr;
		}
	};
}

