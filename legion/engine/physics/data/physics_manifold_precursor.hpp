#pragma once
#include <core/core.hpp>
#include <physics/components/physics_component.hpp>

namespace legion::physics
{
	/** @struct physics_manifold_precursor
	* @brief contains the necessary data to create a physics_manifold
	*/
	struct physics_manifold_precursor
	{
		math::mat4 worldTransform;
		physicsComponent* physicsComp;
		int id;

        physics_manifold_precursor() = default;

		physics_manifold_precursor(math::mat4 pWorldTransform, physicsComponent* pPhysicsComp,int precursorID) :
			worldTransform(pWorldTransform), physicsComp(pPhysicsComp),id(precursorID)
		{

		}
	};

}
