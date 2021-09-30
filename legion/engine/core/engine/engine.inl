#include <core/engine/engine.hpp>
#pragma once

namespace legion::core
{
    template<typename Func>
    inline byte Engine::subscribeToInit(Func&& func)
    {
        initializationSequence().emplace_back(func);
        return 0;
    }

    template<typename Func>
    inline byte Engine::subscribeToShutdown(Func&& func)
    {
        shutdownSequence().emplace(shutdownSequence().begin(), func);
        return 0;
    }

    template <typename ModuleType, typename... Args CNDOXY(inherits_from<ModuleType, Module>)>
    inline void Engine::reportModule(Args && ...args)
    {
        std::unique_ptr<Module> module = std::make_unique<ModuleType>(std::forward<Args>(args)...);
        m_modules[module->priority()].emplace_back(std::move(module));
    }
}
