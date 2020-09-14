
#include <physics/physics_component.hpp>
#include <physics/convexcollider.hpp>

namespace args::physics
{
    void physicsComponent::ConstructConvexHull()
    {

    }

    void physicsComponent::ConstructBox()
    {
    }

    void physicsComponent::AddBox(const cube_collider_params& cubeParams)
    {
        auto newCuboidCollider = std::make_shared<ConvexCollider>();

        newCuboidCollider->CreateBox(cubeParams);

        colliders->push_back(newCuboidCollider);

    }

    void physicsComponent::AddSphere()
    {
    }
}

