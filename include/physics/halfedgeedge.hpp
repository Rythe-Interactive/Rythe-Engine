#pragma once
#include <core/core.hpp>
#include <physics/data/edge_label.hpp>
#include <physics/halfedgeface.hpp>

namespace legion::physics
{
	struct HalfEdgeEdge
	{
		HalfEdgeEdge* pairingEdge = nullptr;
		HalfEdgeEdge * nextEdge = nullptr;
		HalfEdgeEdge* prevEdge = nullptr;

		HalfEdgeFace* face = nullptr;

        EdgeLabel label;
        
		math::vec3* edgePositionPtr;
        std::string id;

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

