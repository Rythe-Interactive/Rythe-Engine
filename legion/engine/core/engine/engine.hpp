#pragma once
#include <core/platform/platform.hpp>
#include <core/engine/module.hpp>
#include <core/types/primitives.hpp>
#include <core/types/meta.hpp>
#include <core/ecs/registry.hpp>
//#include <core/scheduling/scheduler.hpp>
#include <core/events/eventbus.hpp>
//#include <core/defaults/coremodule.hpp>
#include <core/logging/logging.hpp>

#include <map>
#include <vector>
#include <memory>

/**
 * @file engine.hpp
 */

namespace legion::core
{
    /**@class Engine
     * @brief Main top level engine abstraction.
     *        This class allows you to setup the engine with all the necessary modules and settings.
     *        This class also contains all the sub managers of the core systems.
     * @ref legion::core::Module
     * @ref legion::core::ecs::EcsRegistry
     * @ref legion::core::scenes::SceneManager
     * @ref legion::core::scheduling::Scheduler
     */
    class Engine
    {
    private:
        std::vector<char*> m_cliargs;
        std::map<priority_type, std::vector<std::unique_ptr<Module>>, std::greater<>> m_modules;

    public:
        Engine(int argc, char** argv);

        /**@brief Reports an engine module.
         * @tparam ModuleType The module you want to report.
         * @param args The arguments you want to pass to the module constructor.
         * @ref legion::core::Module
         */
        template <typename ModuleType, typename... Args CNDOXY(inherits_from<ModuleType, Module> = 0)>
        void reportModule(Args&&...args);

        /**@brief Calls init on all reported modules and thus engine internals.
         * @note Needs to be called manually if LEGION_ENTRY was not used.
         * @ref legion::core::Module
         */
        void init();

        /**@brief Runs engine loop.
         */
        void run();

        std::vector<char*>& getCliArgs();
    };
}

#include <core/engine/engine.inl>
