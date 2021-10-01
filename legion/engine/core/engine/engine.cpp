#include <core/engine/engine.hpp>
#include <core/defaults/coremodule.hpp>
#include <core/ecs/ecs.hpp>
#include <core/scheduling/scheduling.hpp>
#include <core/events/eventbus.hpp>

namespace legion::core
{
    std::map<priority_type, std::vector<std::unique_ptr<Module>>, std::greater<>> Engine::m_modules;
    int Engine::exitCode = 0;
    argh::parser Engine::cliargs;

    std::atomic_bool Engine::m_shouldRestart = { false };

    multicast_delegate<void()>& Engine::initializationSequence()
    {
        static multicast_delegate<void()> m_initializationSequence;
        return m_initializationSequence;
    }

    multicast_delegate<void()>& Engine::shutdownSequence()
    {
        static multicast_delegate<void()> m_shutdownSequence;
        return m_shutdownSequence;
    }

    void Engine::shutdownModules()
    {
        for (const auto& [priority, moduleList] : m_modules)
            for (auto& module : moduleList)
                module->shutdown();
    }

    Engine::Engine()
    {
        initializationSequence()();

        reportModule<CoreModule>();
    }

    void Engine::run(bool low_power, uint minThreads)
    {
        do
        {
            m_shouldRestart.store(false, std::memory_order_relaxed);

            log::undecoratedInfo("==========================\n"
                                 "| Initializing engine... |\n"
                                 "==========================");

            initializationSequence()();

            {
                auto& logData = log::impl::get();
                async::readwrite_guard guard(logData.threadNamesLock);
                logData.threadNames[std::this_thread::get_id()] = "Initialization";
            }

            for (const auto& [priority, moduleList] : m_modules)
                for (auto& module : moduleList)
                    module->setup();

            for (const auto& [priority, moduleList] : m_modules)
                for (auto& module : moduleList)
                    module->init();
            
            log::undecoratedInfo("==============================\n"
                                 "| Entering main engine loop. |\n"
                                 "==============================");

            exitCode = scheduling::Scheduler::run(low_power, minThreads);

            shutdownSequence()();

        } while (m_shouldRestart.load(std::memory_order_relaxed));
    }

    void Engine::restart()
    {
        m_shouldRestart.store(true, std::memory_order_relaxed);
        
        log::undecoratedInfo("========================\n"
                             "| Restarting engine... |\n"
                             "========================");
        shutdown();
    }

    void Engine::shutdown()
    {
        scheduling::Scheduler::exit(0);
    }

}
