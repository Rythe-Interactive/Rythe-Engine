#include <core/engine/engine.hpp>

namespace legion::core
{
    int Engine::exitCode = 0;
    argh::parser Engine::cliargs;

    Engine::Engine()
    {
    }

    void Engine::init()
    {
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
