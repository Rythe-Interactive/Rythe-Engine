#pragma once
#include <map>
#include <vector>
#include <memory>

#include <argh.h>

#include <core/platform/platform.hpp>
#include <core/engine/module.hpp>
#include <core/types/primitives.hpp>
#include <core/types/meta.hpp>
#include <core/logging/logging.hpp>

/**
 * @file engine.hpp
 */

namespace legion::core
{
    namespace scheduling
    {
        class Scheduler;
    }

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
        friend class legion::core::scheduling::Scheduler;
    private:
        static std::map<priority_type, std::vector<std::unique_ptr<Module>>, std::greater<>> m_modules;

        static std::atomic_bool m_shouldRestart;

        L_NODISCARD static multicast_delegate<void()>& initializationSequence();
        L_NODISCARD static multicast_delegate<void()>& shutdownSequence();

        static void shutdownModules();

    public:
        template<typename Func>
        static byte subscribeToInit(Func&& func);
        template<typename Func>
        static byte subscribeToShutdown(Func&& func);

        static int exitCode;

        static argh::parser cliargs;

        Engine();

        /**@brief Reports an engine module.
         * @tparam ModuleType The module you want to report.
         * @param args The arguments you want to pass to the module constructor.
         * @ref legion::core::Module
         */
        template <typename ModuleType, typename... Args CNDOXY(inherits_from<ModuleType, Module> = 0)>
        void reportModule(Args&&...args);

        /**@brief Runs engine loop.
         */
        void run(bool low_power = false, uint minThreads = 0);

        static void restart();

        static void shutdown();
    };

#define OnEngineInit(Type, Func) ANON_VAR(byte, CONCAT(_onInit_, Type)) = legion::core::Engine::subscribeToInit(Func);
#define OnEngineShutdown(Type, Func) ANON_VAR(byte, CONCAT(_onShutdown_, Type)) = legion::core::Engine::subscribeToShutdown(Func);
}

#include <core/engine/engine.inl>
