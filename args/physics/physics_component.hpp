#pragma once
#include <application/application.hpp>
#include <physics/cube_collider_params.hpp>
#include <vector>

namespace args::physics
{
	struct physicsComponent
	{
		//physics material

		//list of collidables

		//istrigger boolean

        math::vec3 localCenterOfMass{};

		//physics bitmask

        /** @brief Instantiates a ConvexCollider and calls ConstructConvexHullWithMesh on it and passes the given mesh. This
         * ConvexCollider is then added to the list of PhysicsColliders
        */
		void ConstructConvexHull(/*mesh*/);

        /** @brief Instantiates a ConvexCollider and calls ConstructBoxWithMesh on it and passes the given mesh. This
         * ConvexCollider is then added to the list of PhysicsColliders
        */
		void ConstructBox(/*mesh*/);

        /** @brief Instantiates a ConvexCollider and calls CreateBox on it and passes the given mesh. This 
         * ConvexCollider is then added to the list of PhysicsColliders
        */
		void AddBox(const cube_collider_params& cubeParams);

        /** @brief Instantiates a SphereCollider and creates a sphere that encompasses the given mesh. This
         * ConvexCollider is then added to the list of PhysicsColliders
        */
		void AddSphere(/*mesh*/);

	};
}


