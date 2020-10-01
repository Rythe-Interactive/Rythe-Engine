
#include <physics/components/physics_component.hpp>
#include <physics/colliders/convexcollider.hpp>

namespace args::physics
{
    void physicsComponent::calculateNewLocalCenterOfMass()
    {
    }
    void physicsComponent::ConstructConvexHull()
    {

        calculateNewLocalCenterOfMass();
    }

    void physicsComponent::ConstructBox()
    {

        calculateNewLocalCenterOfMass();
    }

    void physicsComponent::AddBox(const cube_collider_params& cubeParams)
    {
        auto cuboidCollider = std::make_shared<ConvexCollider>();

        cuboidCollider->CreateBox(cubeParams);

        colliders->push_back(cuboidCollider);

        calculateNewLocalCenterOfMass();
    }

    void physicsComponent::AddSphere()
    {
        calculateNewLocalCenterOfMass();
    }
}

