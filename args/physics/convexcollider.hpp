#pragma once

#include <physics/physicsimport.h>
#include <physics/physicscollider.hpp>
#include <physics/cube_collider_params.hpp>
#include <physics/HalfEdgeEdge.h>
#include <physics/HalfEdgeFace.h>

namespace args::physics
{
	class ConvexCollider : public PhysicsCollider
	{
	public:

		ConvexCollider();

        ~ConvexCollider();

		virtual void CheckCollision(std::shared_ptr<PhysicsCollider> physicsComponent, physics_manifold* manifold) override;

		virtual void CheckCollisionWith(ConvexCollider* convexCollider, physics_manifold* manifold) override;

        /**@brief Given the current transform of the entity, creates a tight AABB of the collider;
        */
        void UpdateTightAABB(math::mat4 transform);

		/**@brief Constructs a polyhedron-shaped convex hull that encompasses the given mesh.
		*/
		void ConstructConvexHullWithMesh();

		/**@brief Constructs a box-shaped convex hull that encompasses the given mesh.
		*/
		void ConstructBoxWithMesh();

		/**@brief Constructs a box-shaped convex hull based on the given parameters.
		 * @param cubeParams The parameters that will be used to create the box.
		*/
		void CreateBox(const cube_collider_params& cubeParams);

	private:

        HalfEdgeFace* instantiateMeshFace(const std::vector<math::vec3*>& vertices,const math::vec3& faceNormal);

        std::vector<math::vec3> vertices;
        std::vector<HalfEdgeFace*> halfEdgeFaces;

        //feature id container



	};
}


