#pragma once

#include "PhysicsCollider.hpp"

namespace args::physics
{
	class ConvexCollider : public PhysicsCollider
	{
	public:

		ConvexCollider(physicsComponent* colliderOwner);

		virtual void CheckCollision(std::shared_ptr<PhysicsCollider> physicsComponent, physicsManifold* manifold) override;

		virtual void CheckCollisionWith(ConvexCollider* convexCollider, physicsManifold* manifold) override;


	private:

	};
}


