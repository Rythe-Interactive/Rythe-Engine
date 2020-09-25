#pragma once
#include <core/platform/platform.hpp>
#include <core/engine/module.hpp>
#include <core/types/primitives.hpp>
#include <core/types/meta.hpp>
#include <core/ecs/ecsregistry.hpp>
#include <core/scheduling/scheduler.hpp>
#include <core/events/eventbus.hpp>
#include <core/defaults/coremodule.hpp>
#include <core/logging/logging.hpp>

#include <map>
#include <vector>
#include <memory>


/**
 * @file engine.hpp
 */

namespace args::core
{
    /**@class Engine
     * @brief Main top level engine abstraction.
     *        This class allows you to setup the engine with all the necessary modules and settings.
     *        This class also contains all the sub managers of the core systems.
     * @ref args::core::Module
     * @ref args::core::ecs::EcsRegistry
     * @ref args::core::scenes::SceneManager
     * @ref args::core::scheduling::Scheduler
     */
    class Engine
    {
    private:
        std::map<priority_type, std::vector<std::unique_ptr<Module>>, std::greater<>> m_modules;

        events::EventBus m_eventbus;
        ecs::EcsRegistry m_ecs;
        scheduling::Scheduler m_scheduler;

    public:
        Engine() : m_modules(), m_eventbus(), m_ecs(&m_eventbus),
#if defined(ARGS_LOW_POWER)
            m_scheduler(&m_eventbus, true)
#else
            m_scheduler(&m_eventbus, false)
#endif
        {
            reportModule<CoreModule>();
        }

        ~Engine()
        {
            m_modules.clear();
        }

        /**@brief reports an engine module
         * @tparam ModuleType the module you want to report
         * @param s a signal that you want to pass arguments to the constructor of the Module
         * @param args the arguments you want to pass
         * @ref args::core::Module
         */
        template <class ModuleType, class... Args, inherits_from<ModuleType, Module> = 0>
        void reportModule(Args&&...args)
        {
            std::unique_ptr<Module> module = std::make_unique<ModuleType>(std::forward<Args>(args)...);
            module->m_ecs = &m_ecs;
            module->m_scheduler = &m_scheduler;
            module->m_eventBus = &m_eventbus;

            const priority_type priority = module->priority();
            m_modules[priority].emplace_back(std::move(module));
        }

        /**@brief Calls init on all reported modules and thus engine internals.
         * @note Needs to be called manually if ARGS_ENTRY was not used.
         * @ref args::core::Module
         */
        void init()
        {
            log::setup();
          
            for (const auto& [priority, moduleList] : m_modules)
                for (auto& module : moduleList)
                    module->setup();

            for (const auto& [priority, moduleList] : m_modules)
                for (auto& module : moduleList)
                    module->init();
        }

        /**@brief Runs engine loop.
         */
        void run()
        {
            m_scheduler.run();
        }
    };
}
