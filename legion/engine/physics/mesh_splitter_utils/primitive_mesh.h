#pragma once

#include <core/core.hpp>
#include <physics/mesh_splitter_utils/splittable_polygon.h>
#include <physics/mesh_splitter_utils/mesh_splitter_typedefs.hpp>

namespace legion::physics
{
	class PrimitiveMesh
	{
	public:
		PrimitiveMesh(std::vector<std::shared_ptr<SplittablePolygon>>& pPolygons, rendering::material_handle pOriginalMaterial)
			: polygons(std::move(pPolygons)),originalMaterial(pOriginalMaterial)
		{

		}

		ecs::entity_handle InstantiateNewGameObject();

		static void SetECSRegistry(ecs::EcsRegistry* ecs);

	private:

		rendering::material_handle originalMaterial;

		std::vector<std::shared_ptr<SplittablePolygon>> polygons;
		static ecs::EcsRegistry* m_ecs;

	};
}



