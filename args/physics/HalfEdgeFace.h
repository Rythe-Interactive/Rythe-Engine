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

		/** @brief given a function that takes in a HalfEdgeEdge*, 
		* executes the function on each edge connected to 'startEdge'
		*/
		void forEachEdge(args::core::delegate< void(HalfEdgeEdge*)> functionToExecute)
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

		~HalfEdgeFace()
		{
			auto deleteFunc = [](HalfEdgeEdge* edge) { delete edge; };

			forEachEdge(deleteFunc);

		}


	};
}
