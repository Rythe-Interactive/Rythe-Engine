#pragma once
#include <physics/physicsimport.h>
#include <physics/physics_component.hpp>

namespace args::physics
{
	/** @struct physics_manifold_precursor
	* @brief contains the necessary data to create a physics_manifold
	*/
	struct physics_manifold_precursor
	{
		math::mat4 worldTransform;
		ecs::component_handle<physicsComponent> physicsComponentHandle;
		int id;

		physics_manifold_precursor(math::mat4 pWorldTransform,ecs::component_handle<physicsComponent>& pPhysicsCompHandle,int precursorID) :
			worldTransform(pWorldTransform), physicsComponentHandle(pPhysicsCompHandle),id(precursorID)
		{

		}
	};

}
