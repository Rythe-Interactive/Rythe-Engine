#pragma once
#include <application/application.hpp>
#include <vector>

namespace args::physics
{
	struct physicsComponent
	{
		//physics material

		//list of collidables

		
		//istrigger boolean

		//physics bitmask

		void ConstructConvexHull();

		void ConstructOBB();

		void AddBox();

		void AddSphere();

		

	};
}


