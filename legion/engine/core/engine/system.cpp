#include <core/engine/system.hpp>

namespace legion::core
{
    void SystemBase::destroyProcess(id_type procId)
    {
        auto& proc = m_processes.at(procId);

        for (auto hook : proc->hooks())
            schd::Scheduler::unhookProcess(hook, pointer<schd::Process>{ proc.get() });

        if (!proc->inUse())
            m_processes.erase(procId);
    }

    ecs::entity SystemBase::createEntity()
    {
        return ecs::Registry::createEntity();
    }

    ecs::entity SystemBase::createEntity(const std::string& name)
    {
        return ecs::Registry::createEntity(name);
    }

    ecs::entity SystemBase::createEntity(ecs::entity parent)
    {
        return ecs::Registry::createEntity(parent);
    }

    ecs::entity SystemBase::createEntity(ecs::entity parent, const std::string& name)
    {
        return ecs::Registry::createEntity(name, parent);
    }

    ecs::entity SystemBase::createEntity(const std::string& name, ecs::entity parent)
    {
        return ecs::Registry::createEntity(name, parent);
    }

    ecs::entity SystemBase::createEntity(ecs::entity parent, const serialization::entity_prototype& prototype)
    {
        return ecs::Registry::createEntity(parent, prototype);
    }

    ecs::entity SystemBase::createEntity(const serialization::entity_prototype& prototype)
    {
        return ecs::Registry::createEntity(prototype);
    }

    void SystemBase::raiseEvent(events::event_base& value)
    {
        events::EventBus::raiseEvent(value);
    }

    void SystemBase::raiseEventUnsafe(events::event_base& value, id_type id)
    {
        events::EventBus::raiseEventUnsafe(value, id);
    }

}
