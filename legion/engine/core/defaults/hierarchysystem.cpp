#include <core/defaults/hierarchysystem.hpp>

void legion::core::HierarchySystem::onPositionModified(events::component_modification<position>* event)
{
    position diff = event->newValue - event->oldValue;
    for (auto child : event->entity.children())
        child.write_component<position>(child.read_component<position>() + diff);
}

void legion::core::HierarchySystem::onRotationModified(events::component_modification<rotation>* event)
{
    rotation diff = event->newValue * math::inverse(event->oldValue);
    position p = event->entity.read_component<position>();
    for (auto child : event->entity.children())
    {
        child.write_component<position>(p + (math::toMat3(diff) * (child.read_component<position>() - p)));
        child.write_component<rotation>(diff * child.read_component<rotation>());
    }
}

void legion::core::HierarchySystem::onScaleModified(events::component_modification<scale>* event)
{
    scale diff = event->newValue / event->oldValue;
    position p = event->entity.read_component<position>();
    for (auto child : event->entity.children())
    {
        child.write_component<position>(p + (diff * (child.read_component<position>() - p)));
        child.write_component<scale>(child.read_component<scale>() * diff);
    }
}

void legion::core::HierarchySystem::setup()
{
    bindToEvent<events::component_modification<position>, &HierarchySystem::onPositionModified>();
    bindToEvent<events::component_modification<rotation>, &HierarchySystem::onRotationModified>();
    bindToEvent<events::component_modification<scale>, &HierarchySystem::onScaleModified>();
}
