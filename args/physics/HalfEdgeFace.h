#pragma once
#include <physics/HalfEdgeEdge.h>

namespace args::physics
{
	struct HalfEdgeFace
	{
		HalfEdgeEdge* startEdge;
		math::vec3 normal;

		HalfEdgeFace(HalfEdgeEdge* newStartEdge,math::vec3 newNormal) : startEdge{newStartEdge} , normal{newNormal}
		{

		}

		~HalfEdgeFace()
		{
			HalfEdgeEdge* edgeToDelete = startEdge;

			//edgeToDelete will eventually go back to "startEdge", ending the loop
			while (edgeToDelete)
			{
				HalfEdgeEdge* nextEdgeToDelete = edgeToDelete->nextEdge;
				delete edgeToDelete;

				edgeToDelete = nextEdgeToDelete;
			
			}

		}


	};
}
