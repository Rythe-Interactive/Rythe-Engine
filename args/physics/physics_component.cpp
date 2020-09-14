
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
        auto cuboidCollider = std::make_shared<ConvexCollider>();

        cuboidCollider->CreateBox(cubeParams);

        colliders->push_back(cuboidCollider);

    }

    void physicsComponent::AddSphere()
    {
    }
}

