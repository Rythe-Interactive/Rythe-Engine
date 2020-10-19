#pragma once
#include <core/platform/platform.hpp>
#include <core/types/primitives.hpp>
#include <core/types/type_util.hpp>
#include <core/ecs/ecsregistry.hpp>
#include <core/scheduling/scheduler.hpp>
#include <core/scheduling/process.hpp>
#include <core/events/eventbus.hpp>
#include <core/time/time.hpp>
#include <memory>

namespace legion::core
{
    class SystemBase
    {
        friend class Engine;
    protected:
        static ecs::EcsRegistry* m_ecs;
        static scheduling::Scheduler* m_scheduler;
        static events::EventBus* m_eventBus;

        sparse_map<id_type, std::unique_ptr<scheduling::Process>> m_processes;

    public:
        const id_type id;
        const std::string name;

        SystemBase(id_type id, const std::string& name) : id(id), name(name) {}

        virtual void setup() LEGION_PURE;

        virtual ~SystemBase() = default;
    };

    template<typename SelfType>
    class System : public SystemBase
    {
    protected:
        template <void(SelfType::* func_type)(time::time_span<fast_time>), size_type charc>
        void createProcess(const char(&processChainName)[charc], time::time_span<fast_time> interval = 0)
        {
            std::string name = std::string(processChainName) + undecoratedTypeName<SelfType>() + std::to_string(interval) + std::to_string(force_cast<intptr_t>(func_type)[0]);
            id_type id = nameHash(name);
            std::unique_ptr<scheduling::Process> process = std::make_unique<scheduling::Process>(name, id, interval);
            process->setOperation(delegate<void(time::time_span<fast_time>)>::create<SelfType, func_type>((SelfType*)this));
            m_processes.insert(id, std::move(process));

            m_scheduler->hookProcess<charc>(processChainName, m_processes[id].get());
        }

        void createProcess(cstring processChainName, delegate<void(time::time_span<fast_time>)>&& operation, time::time_span<fast_time> interval = 0)
        {
            std::string name = std::string(processChainName) + undecoratedTypeName<SelfType>() + std::to_string(interval);
            id_type id = nameHash(name);

            std::unique_ptr<scheduling::Process> process = std::make_unique<scheduling::Process>(name, id, interval);
            process->setOperation(std::forward<delegate<void(time::time_span<fast_time>)>>(operation));
            m_processes.insert(id, std::move(process));

            m_scheduler->hookProcess(processChainName, m_processes[id].get());
        }

        void destroyProcess(cstring processChainName, time::time_span<fast_time> interval = 0)
        {
            std::string name = std::string(processChainName) + undecoratedTypeName<SelfType>() + std::to_string(interval);
            id_type id = nameHash(name);
            m_scheduler->unhookProcess(processChainName, m_processes[id].get());
            if (!m_processes[id]->inUse())
                m_processes.erase(id);
        }

        void waitForSync()
        {
            m_scheduler->waitForProcessSync();
        }

        /**@brief Create a new entity and return the handle.
         */
        L_NODISCARD ecs::entity_handle createEntity()
        {
            return m_ecs->createEntity();
        }

        template<typename... component_types>
        L_NODISCARD ecs::EntityQuery createQuery()
        {
            return m_ecs->createQuery<component_types...>();
        }

        template<typename event_type, typename... Args, inherits_from<event_type, events::event<event_type>> = 0>
        void raiseEvent(Args... arguments)
        {
            m_eventBus->raiseEvent<event_type>(arguments...);
        }

        template<typename event_type, inherits_from<event_type, events::event<event_type>> = 0>
        L_NODISCARD bool checkEvent() const
        {
            return m_eventBus->checkEvent<event_type>();
        }

        template<typename event_type, inherits_from<event_type, events::event<event_type>> = 0>
        L_NODISCARD size_type getEventCount() const
        {
            return m_eventBus->getEventCount<event_type>();
        }

        template<typename event_type, inherits_from<event_type, events::event<event_type>> = 0>
        L_NODISCARD const event_type& getEvent(index_type index = 0) const
        {
            return m_eventBus->getEvent<event_type>(index);
        }

        template<typename event_type, inherits_from<event_type, events::event<event_type>> = 0>
        L_NODISCARD const event_type& getLastEvent() const
        {
            return m_eventBus->getLastEvent<event_type>();
        }

        template<typename event_type, inherits_from<event_type, events::event<event_type>> = 0>
        void clearEvent(index_type index = 0)
        {
            m_eventBus->clearEvent<event_type>(index);
        }

        template<typename event_type, inherits_from<event_type, events::event<event_type>> = 0>
        void clearLastEvent()
        {
            m_eventBus->clearLastEvent<event_type>();
        }

        template <typename event_type, void(SelfType::* func_type)(event_type*), inherits_from<event_type, events::event<event_type>> = 0>
        void bindToEvent()
        {
            m_eventBus->bindToEvent<event_type>(delegate<void(event_type*)>::template create<SelfType, func_type>(static_cast<SelfType*>(this)));
        }

        template<typename event_type, inherits_from<event_type, events::event<event_type>> = 0>
        void bindToEvent(delegate<void(event_type*)> callback)
        {
            m_eventBus->bindToEvent<event_type>(callback);
        }

    public:
        System() : SystemBase(typeHash<SelfType>(), undecoratedTypeName<SelfType>()) {}
    };
}
