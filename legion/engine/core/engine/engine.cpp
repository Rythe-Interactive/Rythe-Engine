#include <core/engine/engine.hpp>
#include <core/defaults/coremodule.hpp>
#include <core/ecs/ecs.hpp>
#include <core/scheduling/scheduling.hpp>
#include <core/events/eventbus.hpp>

namespace legion::core
{
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

    Engine::Engine()
    {
        ecs::Registry::init();
        compute::Context::init();
        scheduling::Scheduler::init();

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

            ecs::Registry::init();
            compute::Context::init();
            scheduling::Scheduler::init();
            initializationSequence()();

            {
                async::readwrite_guard guard(log::impl::threadNamesLock);
                log::impl::threadNames[std::this_thread::get_id()] = "Initialization";
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
            scheduling::Scheduler::shutdown();
            compute::Context::shutdown();
            ecs::Registry::shutdown();

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
