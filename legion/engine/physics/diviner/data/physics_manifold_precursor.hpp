#pragma once
#include <core/core.hpp>
#include <physics/diviner/components/physics_component.hpp>

namespace legion::physics
{
    /** @struct physics_manifold_precursor
    * @brief contains the necessary data to create a physics_manifold.
    * Only exist within one physics timestep.
    */
    struct physics_manifold_precursor
    {
        math::mat4 worldTransform;
        physicsComponent* physicsComp = nullptr;
        id_type id;
        ecs::entity entity;

        physics_manifold_precursor() = default;

        physics_manifold_precursor(math::mat4 pWorldTransform, physicsComponent* pPhysicsComp, id_type precursorID, ecs::entity entity) :
            worldTransform(pWorldTransform), physicsComp(pPhysicsComp), id(precursorID), entity(entity)
        {

        }

        bool operator==(const physics_manifold_precursor& rhs)
        {
            return entity == rhs.entity;
        }
    };

}
