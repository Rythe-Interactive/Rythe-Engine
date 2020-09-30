#pragma once
#include <physics/physicsimport.h>
#include <physics/data/penetrationquery.h>
#include <physics/halfedgeedge.hpp>

namespace args::physics
{
	class EdgePenetrationQuery : public PenetrationQuery
	{
	public:
		HalfEdgeEdge* refEdge = nullptr;
		HalfEdgeEdge* incEdge = nullptr;

		EdgePenetrationQuery() = default;

		EdgePenetrationQuery(HalfEdgeEdge* pRefEdge, HalfEdgeEdge* pIncEdge,
			math::vec3& pFaceCentroid, math::vec3& pNormal, float& pPenetration, bool pIsARef);

		virtual void populateContactList(physics_manifold& manifold);
	};
}

