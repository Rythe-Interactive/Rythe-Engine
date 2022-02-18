#pragma once
#include <core/events/event.hpp>
#include <physics/diviner/data/physics_manifold.hpp>

namespace legion::physics {


    template <class T>
    struct collision_event_base : public events::event<T>
    {
        collision_event_base(physics_manifold* m, float d) : manifold(m) , physics_delta(d) {}

        collision_event_base(const collision_event_base& other) = default;
        collision_event_base(collision_event_base&& other) noexcept = default;
        collision_event_base& operator=(const collision_event_base& other) = default;
        collision_event_base& operator=(collision_event_base&& other) noexcept = default;

        physics_manifold* manifold;


        /** @brief utility function to quickly get the participating entities
         *         of this collision.
         */

        L_NODISCARD std::pair<ecs::entity,ecs::entity> participants() const
        {
            return std::make_pair(
                manifold->entityA,
                manifold->entityB
            );
        }

        /** @brief gets the centers of the colliders in local space
         */
        L_NODISCARD std::pair<math::vec3,math::vec3> centers() const noexcept
        {
            return std::make_pair(
                manifold->colliderA->GetLocalCentroid(),
                manifold->colliderB->GetLocalCentroid()
            );
        }

        /** @brief gets the colliders themselves
         */
        L_NODISCARD std::pair<PhysicsCollider*,
                             PhysicsCollider*> colliders() const noexcept
        {
            return std::make_pair(manifold->colliderA, manifold->colliderB);
        }

        /** @brief gets the transform of the colliding bodys themselves
         */
        L_NODISCARD std::pair<transform,transform> transforms() const
        {
            return std::apply([](ecs::entity_handle a,ecs::entity_handle b)
            {
                return std::make_pair(
                    transform(a.get_component_handles<transform>()),
                    transform(b.get_component_handles<transform>())
                );
            }, participants());
        }


        float physics_delta;

    };

    /** @class trigger_event
     *  @brief This event is raised when two triggers in the physics-system collide.
     *         It contains the collision event in the manifold and the delta time of
     *         the physics system
     */
    struct trigger_event : public collision_event_base<trigger_event>
    {
        using collision_event_base<trigger_event>::collision_event_base;
    };

    /** @class collision_event
     *  @brief This event is raised when two triggers in the physics-system collide.
     *         It contains the collision event in the manifold and the delta time of
     *         the physics system
     */
    struct collision_event : public collision_event_base<collision_event>
    {
        using collision_event_base<collision_event>::collision_event_base;
    };
}
