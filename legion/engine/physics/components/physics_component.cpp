
#include <physics/components/physics_component.hpp>
#include <physics/colliders/convexcollider.hpp>

namespace legion::physics
{
    void physicsComponent::calculateNewLocalCenterOfMass()
    {
    }

    std::shared_ptr<ConvexCollider> physicsComponent::ConstructConvexHull(legion::core::mesh_handle& meshHandle)
    {
        auto collider = std::make_shared<ConvexCollider>();

        collider->ConstructConvexHullWithMesh(meshHandle);

        colliders->push_back(collider);

        calculateNewLocalCenterOfMass();

        return collider;
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

