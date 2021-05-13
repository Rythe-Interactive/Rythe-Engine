#include <core/engine/engine.hpp>
#include <core/defaults/coremodule.hpp>

namespace legion::core
{
    int Engine::exitCode = 0;
    argh::parser Engine::cliargs;

    Engine::Engine()
    {
        reportModule<CoreModule>();
    }

    void Engine::init()
    {
        {
            async::readwrite_guard guard(log::impl::thread_names_lock);
            log::impl::thread_names[std::this_thread::get_id()] = "Initialization";
        }

        for (const auto& [priority, moduleList] : m_modules)
            for (auto& module : moduleList)
                module->setup();

        for (const auto& [priority, moduleList] : m_modules)
            for (auto& module : moduleList)
                module->init();
    }

    void Engine::run(bool low_power, uint minThreads)
    {
       exitCode = scheduling::Scheduler::run(low_power, minThreads);
    }
}
