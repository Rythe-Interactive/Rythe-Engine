#pragma once

#include <physics/physicsimport.h>
#include <physics/data/physics_manifold.hpp>

namespace args::physics
{
	class PenetrationQuery
	{
	public:

		math::vec3 faceCentroid,normal;
		float penetration;
		bool isARef;

		PenetrationQuery(math::vec3& pFaceCentroid,math::vec3& pNormal,float& pPenetration,bool pIsARef) :
			faceCentroid(pFaceCentroid),normal(pNormal),penetration(pPenetration),isARef(pIsARef)
		{

		}

		bool operator< (PenetrationQuery const& b)
		{
			return  this->penetration < (b.penetration);
		}
		

		virtual void populateContactList(physics_manifold& manifold) = 0;


	};

}

