#pragma once
#include <core/platform/platform.hpp>
#include <core/engine/module.hpp>
#include <core/types/primitives.hpp>
#include <core/types/meta.hpp>
#include <core/async/spinlock.hpp>
#include <core/containers/pointer.hpp>

#include <argh.h>

#include <map>
#include <vector>
#include <memory>

/**
 * @file engine.hpp
 */

namespace legion::core
{
    namespace scheduling
    {
        class Scheduler;
    }

    class Engine;

    struct this_engine
    {
        friend class Engine;
        friend class scheduling::Scheduler;
    private:
        thread_local static Engine* m_ptr;

    public:
        static pointer<Engine> get_context();
        static int& exit_code();
        static argh::parser& cliargs();

        static void restart();
        static void shutdown();
    };

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
        std::map<priority_type, std::vector<std::unique_ptr<Module>>, std::greater<>> m_modules;

        std::atomic_bool m_shouldRestart;

        static size_type m_runningInstances;
        static async::spinlock m_startupShutdownLock;

        L_NODISCARD static multicast_delegate<void()>& initializationSequence();
        L_NODISCARD static multicast_delegate<void()>& shutdownSequence();

        void shutdownModules();

    public:
        template<typename Func>
        static byte subscribeToInit(Func&& func);
        template<typename Func>
        static byte subscribeToShutdown(Func&& func);

        template<typename SubSystem>
        static byte reportSubSystem();

        int exitCode;

        argh::parser cliargs;

        Engine(int argc, char** argv);
        Engine();
        NO_DEF_CTOR_RULE5(Engine);

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

        void restart();

        void shutdown();
    };

#define OnEngineInit(Type, Func) ANON_VAR(byte, CONCAT(_onInit_, Type)) = legion::core::Engine::subscribeToInit(Func);
#define OnEngineShutdown(Type, Func) ANON_VAR(byte, CONCAT(_onShutdown_, Type)) = legion::core::Engine::subscribeToShutdown(Func);

#define ReportSubSystem(Type) ANON_VAR(byte, CONCAT(_reportSubSystem_, Type)) = legion::core::Engine::reportSubSystem<Type>();

}

#include <core/engine/engine.inl>
