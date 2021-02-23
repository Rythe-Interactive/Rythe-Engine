#pragma once
#include <core/core.hpp>
#include <physics/components/physics_component.hpp>

namespace legion::physics
{
    /** @struct physics_manifold_precursor
    * @brief contains the necessary data to create a physics_manifold
    */
    struct physics_manifold_precursor
    {
        math::mat4 worldTransform;
        physicsComponent* physicsComp;
        id_type id;
        ecs::entity_handle entity;

        physics_manifold_precursor() = default;

        physics_manifold_precursor(math::mat4 pWorldTransform, physicsComponent* pPhysicsComp, id_type precursorID, ecs::entity_handle entity) :
            worldTransform(pWorldTransform), physicsComp(pPhysicsComp), id(precursorID), entity(entity)
        {

        }

        bool operator==(const physics_manifold_precursor& rhs)
        {
            return entity == rhs.entity;
        }
    };

}
