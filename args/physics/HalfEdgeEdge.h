#pragma once

#include <physics/physicsimport.h>


namespace args::physics
{
	struct HalfEdgeEdge
	{
		HalfEdgeEdge* pairingEdge;
		HalfEdgeEdge * nextEdge;
		HalfEdgeEdge* prevEdge;

		math::vec3* edgePositionPtr;

		HalfEdgeEdge(math::vec3* newEdgePositionPtr) : edgePositionPtr{ newEdgePositionPtr }
		{

		}

		void setNextAndPrevEdge(HalfEdgeEdge* newPrevEdge,HalfEdgeEdge* newNextEdge)
		{
			nextEdge = newNextEdge;
			prevEdge = newPrevEdge;
		}

		


	};




}

