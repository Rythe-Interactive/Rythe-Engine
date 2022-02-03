#include <core/engine/engine.hpp>
#include <core/common/hash.hpp>
#include <core/defaults/coremodule.hpp>
#include <core/ecs/ecs.hpp>
#include <core/scheduling/scheduling.hpp>
#include <core/events/eventbus.hpp>

namespace legion::core
{
    thread_local Engine* this_engine::m_ptr;

    pointer<Engine> this_engine::get_context()
    {
        return { m_ptr };
    }

    engine_id& this_engine::id()
    {
        return m_ptr->id;
    }

    int& this_engine::exit_code()
    {
        return m_ptr->exitCode;
    }

    argh::parser& this_engine::cliargs()
    {
        return m_ptr->cliargs;
    }

    void this_engine::restart()
    {
        m_ptr->restart();
    }

    void this_engine::shutdown()
    {
        m_ptr->shutdown();
    }

    size_type Engine::m_initializedInstances = 0;
    async::spinlock Engine::m_startupShutdownLock{};

    id_type Engine::generateId()
    {
        static id_type baseId = nameHash("\xabLEGION ENGINE\xbb\r\n\x13\n");
        id_type threadId = std::hash<std::thread::id>{}(std::this_thread::get_id());
        return combine_hash(baseId++, threadId);
    }

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

    Engine::Engine(int argc, char** argv)
        : m_modules(), m_shouldRestart(false), id(generateId()), exitCode(0), cliargs(argc, argv)
    {
        reportModule<CoreModule>();
    }

    Engine::Engine() : m_modules(), m_shouldRestart(false), id(generateId()), exitCode(0), cliargs()
    {
        reportModule<CoreModule>();
    }

    Engine::Engine(Engine&& other)
        : m_modules(std::move(other.m_modules)), m_shouldRestart(other.m_shouldRestart.load(std::memory_order_relaxed)), id(std::move(other.id)), exitCode(0), cliargs(std::move(other.cliargs))
    {
        reportModule<CoreModule>();
    }

    Engine& Engine::operator=(Engine&& other)
    {
        m_modules = std::move(other.m_modules);
        m_shouldRestart.store(other.m_shouldRestart.load(std::memory_order_relaxed), std::memory_order_relaxed);
        id = std::move(other.id);
        exitCode = other.exitCode;
        cliargs = std::move(other.cliargs);
        return *this;
    }

    void Engine::makeCurrentContext()
    {
        this_engine::m_ptr = this;
    }

    void Engine::initialize()
    {
        log::undecoratedInfo(
            "==========================\n"
            "| Initializing engine... |\n"
            "==========================");

        {
            async::lock_guard guard(m_startupShutdownLock);
            if (m_initializedInstances == 0)
                initializationSequence()();

            m_initializedInstances++;
        }

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


        {
            auto& logData = log::impl::get();
            async::readwrite_guard guard(logData.threadNamesLock);
            logData.threadNames[std::this_thread::get_id()] = "Main thread: " + std::to_string(id.value());
        }
    }

    void Engine::uninitialize()
    {
        async::lock_guard guard(m_startupShutdownLock);
        m_initializedInstances--;

        if (m_initializedInstances == 0)
            shutdownSequence()();
    }

    void Engine::run(bool low_power, uint minThreads)
    {
        makeCurrentContext();
        do
        {
            m_shouldRestart.store(false, std::memory_order_relaxed);

            initialize();

            log::undecoratedInfo(
                "==============================\n"
                "| Entering main engine loop. |\n"
                "==============================");

            exitCode = scheduling::Scheduler::run(*this, low_power, minThreads);

            uninitialize();

        } while (m_shouldRestart.load(std::memory_order_relaxed));
    }

    void Engine::restart()
    {
        m_shouldRestart.store(true, std::memory_order_relaxed);

        log::undecoratedInfo(
            "========================\n"
            "| Restarting engine... |\n"
            "========================");
        shutdown();
    }

    void Engine::shutdown()
    {
        scheduling::Scheduler::exit(exitCode);
    }
}
