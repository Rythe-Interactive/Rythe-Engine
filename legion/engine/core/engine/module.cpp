#include <core/engine/module.hpp>

namespace legion::core
{
    ecs::EcsRegistry* Module::m_ecs;
    scheduling::Scheduler* Module::m_scheduler;
    events::EventBus* Module::m_eventBus;
}
