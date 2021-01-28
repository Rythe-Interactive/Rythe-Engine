#include <core/defaults/hierarchysystem.hpp>

void legion::core::HierarchySystem::onPositionModified(events::component_modification<position>* event)
{
    OPTICK_EVENT();
    if (!event->entity.has_component<hierarchy>())
        return;

    position diff = event->newValue - event->oldValue;
    auto children = event->entity.children();

    m_scheduler->queueJobs(children.size(), [&]()
        {
            auto& child = children[async::this_job::get_id()];
            child.write_component<position>(child.read_component<position>() + diff);
        }).wait();
}

void legion::core::HierarchySystem::onRotationModified(events::component_modification<rotation>* event)
{
    OPTICK_EVENT();
    if (!event->entity.has_component<hierarchy>())
        return;

    rotation diff = event->newValue * math::inverse(event->oldValue);
    position pos = event->entity.read_component<position>();
    auto children = event->entity.children();
    m_scheduler->queueJobs(children.size(), [&]()
        {
            auto& child = children[async::this_job::get_id()];
            child.write_component<position>(pos + (math::toMat3(diff) * (child.read_component<position>() - pos)));
            child.write_component<rotation>(diff * child.read_component<rotation>());
        }).wait();
}

void legion::core::HierarchySystem::onScaleModified(events::component_modification<scale>* event)
{
    OPTICK_EVENT();
    if (!event->entity.has_component<hierarchy>())
        return;

    scale diff = event->newValue / event->oldValue;
    position pos = event->entity.read_component<position>();
    auto children = event->entity.children();
    m_scheduler->queueJobs(children.size(), [&]()
        {
            auto& child = children[async::this_job::get_id()];
            child.write_component<position>(pos + (diff * (child.read_component<position>() - pos)));
            child.write_component<scale>(child.read_component<scale>() * diff);
        }).wait();
}

void legion::core::HierarchySystem::onPositionBulkModified(events::bulk_component_modification<position>* event)
{
    OPTICK_EVENT();
    const auto& entities = event->entities;
    const auto& oldValues = event->oldValues;
    const auto& newValues = event->newValues;

    size_type count = entities.size();

    ecs::component_container<position> diffs;
    diffs.resize(count);

    std::vector<byte> hasChildren;
    hasChildren.resize(count);

    std::vector<ecs::entity_set> children;
    children.resize(count);

    m_scheduler->queueJobs(count, [&]()
        {
            auto i = async::this_job::get_id();
            if (entities[i].has_component<hierarchy>())
            {
                children[i] = entities[i].read_component<hierarchy>().children;
                hasChildren[i] = children[i].size() > 0;
            }
            else
                hasChildren[i] = false;
        }).then(count, [&]()
            {
                auto i = async::this_job::get_id();
                if (hasChildren[i])
                    diffs[i] = newValues[i] - oldValues[i];
            }).wait();

        for (int i = 0; i < count; i++)
        {
            OPTICK_EVENT("Update child");
            if (!hasChildren[i])
                continue;

            m_scheduler->queueJobs(children[i].size(), [&]()
                {
                    auto& child = children[i][async::this_job::get_id()];
                    child.write_component<position>(child.read_component<position>() + diffs[i]);
                }).wait();
        }
}

void legion::core::HierarchySystem::onRotationBulkModified(events::bulk_component_modification<rotation>* event)
{
    OPTICK_EVENT();
    const auto& entities = event->entities;
    const auto& oldValues = event->oldValues;
    const auto& newValues = event->newValues;

    size_type count = entities.size();

    ecs::component_container<rotation> diffs;
    diffs.resize(count);

    std::vector<byte> hasChildren;
    hasChildren.resize(count);

    std::vector<ecs::entity_set> children;
    children.resize(count);

    m_scheduler->queueJobs(count, [&]()
        {
            auto i = async::this_job::get_id();
            if (entities[i].has_component<hierarchy>())
            {
                children[i] = entities[i].read_component<hierarchy>().children;
                hasChildren[i] = children[i].size() > 0;
            }
            else
                hasChildren[i] = false;
        }).then(count, [&]()
            {
                auto i = async::this_job::get_id();
                if (hasChildren[i])
                    diffs[i] = newValues[i] * math::inverse(oldValues[i]);
            }).wait();

        for (int i = 0; i < count; i++)
        {
            OPTICK_EVENT("Update child");
            if (!hasChildren[i])
                continue;

            position pos = entities[i].read_component<position>();

            m_scheduler->queueJobs(children[i].size(), [&]()
                {
                    auto& child = children[i][async::this_job::get_id()];
                    child.write_component<position>(pos + (math::toMat3(diffs[i]) * (child.read_component<position>() - pos)));
                    child.write_component<rotation>(diffs[i] * child.read_component<rotation>());
                }).wait();
        }
}

void legion::core::HierarchySystem::onScaleBulkModified(events::bulk_component_modification<scale>* event)
{
    OPTICK_EVENT();
    const auto& entities = event->entities;
    const auto& oldValues = event->oldValues;
    const auto& newValues = event->newValues;

    size_type count = entities.size();

    ecs::component_container<scale> diffs;
    diffs.resize(count);

    std::vector<byte> hasChildren;
    hasChildren.resize(count);

    std::vector<ecs::entity_set> children;
    children.resize(count);

    m_scheduler->queueJobs(count, [&]()
        {
            auto i = async::this_job::get_id();
            if (entities[i].has_component<hierarchy>())
            {
                children[i] = entities[i].read_component<hierarchy>().children;
                hasChildren[i] = children[i].size() > 0;
            }
            else
                hasChildren[i] = false;
        }).then(count, [&]()
            {
                auto i = async::this_job::get_id();
                if (hasChildren[i])
                    diffs[i] = newValues[i] / oldValues[i];
            }).wait();

        for (int i = 0; i < count; i++)
        {
            OPTICK_EVENT("Update child");

            if (!hasChildren[i])
                continue;

            position pos = entities[i].read_component<position>();

            m_scheduler->queueJobs(children[i].size(), [&]()
                {
                    auto& child = children[i][async::this_job::get_id()];
                    child.write_component<position>(pos + (diffs[i] * (child.read_component<position>() - pos)));
                    child.write_component<scale>(child.read_component<scale>() * diffs[i]);
                }).wait();
        }
}

void legion::core::HierarchySystem::setup()
{
    bindToEvent<events::component_modification<position>, &HierarchySystem::onPositionModified>();
    bindToEvent<events::component_modification<rotation>, &HierarchySystem::onRotationModified>();
    bindToEvent<events::component_modification<scale>, &HierarchySystem::onScaleModified>();
    bindToEvent<events::bulk_component_modification<position>, &HierarchySystem::onPositionBulkModified>();
    bindToEvent<events::bulk_component_modification<rotation>, &HierarchySystem::onRotationBulkModified>();
    bindToEvent<events::bulk_component_modification<scale>, &HierarchySystem::onScaleBulkModified>();
}
