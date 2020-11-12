#pragma once
#include <core/events/event.hpp>
#include <physics/data/physics_manifold.hpp>

namespace legion::physics {
class TriggerEvent : public events::event<TriggerEvent>
{
    public:
    TriggerEvent(physics_manifold m,float d) : manifold(std::move(m)) , physics_delta(d) {}

    TriggerEvent(const TriggerEvent& other) = default;
    TriggerEvent(TriggerEvent&& other) noexcept = default;
    TriggerEvent& operator=(const TriggerEvent& other) = default;
    TriggerEvent& operator=(TriggerEvent&& other) noexcept = default;

    physics_manifold manifold;
    float physics_delta;

};
}
