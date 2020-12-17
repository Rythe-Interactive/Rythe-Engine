#include <core/engine/system.hpp>

namespace legion::core
{
    ecs::EcsRegistry* SystemBase::m_ecs;
    scheduling::Scheduler* SystemBase::m_scheduler;
    events::EventBus* SystemBase::m_eventBus;
    ecs::entity_handle SystemBase::world = ecs::entity_handle(world_entity_id);
}
