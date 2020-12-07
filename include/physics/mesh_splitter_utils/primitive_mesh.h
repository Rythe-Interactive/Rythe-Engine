#pragma once

#include <core/core.hpp>
#include <physics/mesh_splitter_utils/splittable_polygon.h>
#include <physics/mesh_splitter_utils/mesh_splitter_typedefs.hpp>

namespace legion::physics
{
	class PrimitiveMesh
	{
	public:
		PrimitiveMesh(std::vector<std::shared_ptr<SplittablePolygon>>& pPolygons) 
			: polygons(std::move(pPolygons))
		{

		}

		ecs::entity_handle InstantiateNewGameObject();

	private:

		std::vector<std::shared_ptr<SplittablePolygon>> polygons;


	};
}



