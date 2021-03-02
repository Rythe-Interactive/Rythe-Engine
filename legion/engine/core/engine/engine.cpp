#include <core/engine/engine.hpp>

namespace legion::core
{
    Engine::Engine(int argc, char** argv) : m_cliargs(argv, argv + argc), m_modules()
    {
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

    std::vector<char*>& Engine::getCliArgs()
    {
        return m_cliargs;
    }

}
