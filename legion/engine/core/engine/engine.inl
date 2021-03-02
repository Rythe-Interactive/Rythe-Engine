#include <core/engine/engine.hpp>
#pragma once

namespace legion::core
{
    template <typename ModuleType, typename... Args CNDOXY(inherits_from<ModuleType, Module>)>
    inline void Engine::reportModule(Args && ...args)
    {
        std::unique_ptr<Module> module = std::make_unique<ModuleType>(std::forward<Args>(args)...);
        m_modules[module->priority()].emplace_back(std::move(module));
    }
}
