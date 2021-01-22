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

namespace legion::core
{
    /**@class Module
     * @brief interface for engine-modules, must be implemented
     * @ref legion::core::Engine::reportModule<T,...>()
     */
    class Module
    {
        friend class Engine;
    private:
        sparse_map<id_type, std::unique_ptr<SystemBase>> m_systems;

        void init()
        {
            for (auto [_, system] : m_systems)
                system->setup();
        };

    protected:
        static ecs::EcsRegistry* m_ecs;
        static scheduling::Scheduler* m_scheduler;
        static events::EventBus* m_eventBus;

        template<size_type charc>
        void addProcessChain(const char(&name)[charc])
        {
            m_scheduler->addProcessChain<charc>(name);
        }

        template<typename SystemType, typename... Args CNDOXY(inherits_from<SystemType, System<SystemType>> = 0)>
        void reportSystem(Args&&... args)
        {
            OPTICK_EVENT();
            m_systems.insert(typeHash<SystemType>(), std::make_unique<SystemType>(std::forward<Args>(args)...));
        }

        template<typename component_type>
        void reportComponentType()
        {
            m_ecs->reportComponentType<component_type>();
        }

    public:
        virtual void setup() LEGION_PURE;

        /**@brief determines the execution priority of this module
         * @ref LEGION_IMPURE_RETURN
         * @returns priority_type signed int8 higher is higher priority and get called first
         * @note default priority of the engine is 0.
         * @note call order for modules with the same priority is undefined.
         */
        virtual priority_type priority() LEGION_IMPURE_RETURN(default_priority);

        virtual ~Module() = default;
    };
}
