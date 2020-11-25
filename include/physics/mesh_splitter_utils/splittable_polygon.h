#pragma once
#include <core/core.hpp>
#include <physics/mesh_splitter_utils/mesh_half_edge.hpp>

namespace legion::physics
{
	class SplittablePolygon
	{
	public:
		SplittablePolygon(std::vector<std::shared_ptr<MeshHalfEdge>> pEdgesInMesh);

		std::vector<std::shared_ptr<MeshHalfEdge>>& GetMeshEdges();

		void calculateLocalCentroid();

		//mostly used for debugging
		math::vec3 localCentroid{ 0,0,0 };
		math::color debugColor;

	private:

		

		std::vector<std::shared_ptr<MeshHalfEdge>> edgesInMesh;


	};
}


