#include <core/engine/system.hpp>
#pragma once

namespace legion::core
{
    template<typename SelfType>
    template <void(SelfType::* func_type)(time::span), size_type charc>
    inline L_ALWAYS_INLINE id_type System<SelfType>::createProcess(const char(&processChainName)[charc], time::span interval)
    {
        std::string name = std::string(processChainName) + nameOfType<SelfType>() + std::to_string(interval) + std::to_string(force_cast<intptr_t>(func_type)[0]);
        id_type id = nameHash(name);
        std::unique_ptr<schd::Process> process = std::make_unique<schd::Process>(name, id, interval);
        process->setOperation(delegate<void(time::span)>::from<SelfType, func_type>(reinterpret_cast<SelfType*>(this)));
        m_processes.emplace(id, std::move(process));

        schd::Scheduler::hookProcess<charc>(processChainName, pointer<schd::Process>{ m_processes[id].get() });
        return id;
    }

    template<typename SelfType>
    inline L_ALWAYS_INLINE void System<SelfType>::destroyProcess(id_type procId)
    {
        auto& proc = m_processes.at(procId);

        for (auto hook : proc->hooks())
            schd::Scheduler::unhookProcess(hook, pointer{ proc.get() });

        if (!proc->inUse())
            m_processes.erase(procId);
    }

    template<typename SelfType>
    template<typename event_type, void(SelfType::* func_type)(event_type&) CNDOXY(typename)>
    inline L_ALWAYS_INLINE id_type System<SelfType>::bindToEvent()
    {
        id_type id = combine_hash(event_type::id, *force_cast<id_type>(func_type));

        auto temp = delegate<void(event_type&)>::template from<SelfType, func_type>(reinterpret_cast<SelfType*>(this));
        auto& del = m_bindings.try_emplace(id, reinterpret_cast<delegate<void(events::event_base&)>&&>(std::move(temp))).first->second;

        events::EventBus::bindToEvent<event_type>(reinterpret_cast<delegate<void(event_type&)>&>(del));

        return id;
    }

    template<typename SelfType>
    template <typename event_type CNDOXY(typename)>
    inline L_ALWAYS_INLINE void System<SelfType>::unbindFromEvent(id_type bindingId)
    {
        events::EventBus::unbindFromEvent<event_type>(reinterpret_cast<delegate<void(event_type&)>&>(m_bindings.at(bindingId)));
        m_bindings.erase(bindingId);
    }

    template<typename SelfType>
    inline L_ALWAYS_INLINE ecs::entity System<SelfType>::createEntity()
    {
        return ecs::Registry::createEntity();
    }

    template<typename SelfType>
    inline L_ALWAYS_INLINE ecs::entity System<SelfType>::createEntity(ecs::entity parent)
    {
        return ecs::Registry::createEntity(parent);
    }

    template<typename SelfType>
    inline L_ALWAYS_INLINE ecs::entity System<SelfType>::createEntity(ecs::entity parent, const serialization::entity_prototype& prototype)
    {
        return ecs::Registry::createEntity(parent, prototype);
    }

    template<typename SelfType>
    inline L_ALWAYS_INLINE ecs::entity System<SelfType>::createEntity(const serialization::entity_prototype& prototype)
    {
        return ecs::Registry::createEntity(prototype);
    }

    template<typename SelfType>
    template<typename event_type, typename... Args CNDOXY(typename)>
    inline L_ALWAYS_INLINE void System<SelfType>::raiseEvent(Args&&... arguments)
    {
        events::EventBus::raiseEvent<event_type>(std::forward<Args>(arguments)...);
    }

    template<typename SelfType>
    inline L_ALWAYS_INLINE void System<SelfType>::raiseEvent(events::event_base& value)
    {
        events::EventBus::raiseEvent(value);
    }

    template<typename SelfType>
    inline L_ALWAYS_INLINE void System<SelfType>::raiseEventUnsafe(events::event_base& value, id_type id)
    {
        events::EventBus::raiseEventUnsafe(value, id);
    }

    template<typename SelfType>
    template<typename Func>
    inline L_ALWAYS_INLINE auto System<SelfType>::queueJobs(size_type count, Func&& func)
    {
        if constexpr (std::is_invocable_v<Func, id_type>)
        {
            return schd::Scheduler::queueJobs(count, [&]() {
                std::invoke(func, async::this_job::get_id());
                });
        }
        else
        {
            static_assert(std::is_invocable_v<Func>, "Func type should be invocable with either an id as a parameter or with no parameters.");
            return schd::Scheduler::queueJobs(count, std::forward<Func>(func));
        }
    }

    template<typename SelfType>
    template<void(SelfType::* func_type)()>
    inline L_ALWAYS_INLINE auto System<SelfType>::queueJobs(size_type count)
    {
        return schd::Scheduler::queueJobs(count, [&]() {
            std::invoke(func_type, this, async::this_job::get_id());
            });
    }

    template<typename SelfType>
    template<void(SelfType::* func_type)(id_type)>
    inline L_ALWAYS_INLINE auto System<SelfType>::queueJobs(size_type count)
    {
        return schd::Scheduler::queueJobs(count, [&]() {
            std::invoke(func_type, this);
            });
    }

}
