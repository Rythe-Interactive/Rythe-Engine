#pragma once
#include <core/types/primitives.hpp>
#include <core/platform/platform.hpp>
#include <core/engine/system.hpp>
#include <core/containers/sparse_map.hpp>
#include <core/ecs/ecsregistry.hpp>
#include <core/scheduling/scheduler.hpp>
#include <core/events/eventbus.hpp>
#include <memory>

/**
 * @file module.hpp
 */

namespace args::core
{
    /**@class Module
     * @brief interface for engine-modules, must be implemented
     * @ref args::core::Engine::reportModule<T,...>()
     */
    class Module
    {
        friend class Engine;
    private:
        ecs::EcsRegistry* m_ecs;
        scheduling::Scheduler* m_scheduler;
        events::EventBus* m_eventBus;

        sparse_map<id_type, std::unique_ptr<SystemBase>> m_systems;

        void init()
        {
            for (auto [_, system] : m_systems)
                system->setup();
        };

    protected:
        template<size_type charc>
        void addProcessChain(const char(&name)[charc])
        {
            m_scheduler->addProcessChain<charc>(name);
        }

        template<typename SystemType, typename... Args, inherits_from<SystemType, System<SystemType>> = 0>
        void reportSystem(Args&&... args)
        {
            std::unique_ptr<SystemBase> system = std::make_unique<SystemType>(std::forward<Args>(args)...);
            system->m_ecs = m_ecs;
            system->m_scheduler = m_scheduler;
            system->m_eventBus = m_eventBus;

            m_systems.insert(typeHash<SystemType>(), std::move(system));
        }

        template<typename component_type>
        void reportComponentType()
        {
            m_ecs->reportComponentType<component_type>();
        }

    public:
        virtual void setup() ARGS_PURE;

        /**@brief determines the execution priority of this module
         * @ref ARGS_IMPURE_RETURN
         * @returns priority_type signed int8 higher is higher priority and get called first
         * @note default priority of the engine is 0.
         * @note call order for modules with the same priority is undefined.
         */
        virtual priority_type priority() ARGS_IMPURE_RETURN(default_priority);

        virtual ~Module() = default;
    };
}
