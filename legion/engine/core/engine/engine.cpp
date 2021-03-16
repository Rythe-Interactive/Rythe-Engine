#include <core/engine/engine.hpp>

namespace legion::core
{
    size_type Engine::exitCode = 0;
    argh::parser Engine::cliargs;

    Engine::Engine(int argc, char** argv) : m_modules()
    {
        cliargs.parse(argc, argv);
        log::setup();
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

    void Engine::run()
    {
        //m_scheduler.run();
    }
}
