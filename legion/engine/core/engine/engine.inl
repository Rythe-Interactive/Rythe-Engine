#include <core/engine/engine.hpp>
#pragma once

namespace legion::core
{
    template<typename Func>
    inline byte Engine::subscribeToInit(Func&& func)
    {
        static_assert(sizeof(func) == sizeof(ptr_type), "Parameter passed to subscribeToInit is not a function.");
        static std::unordered_set<ptr_type> items;

        ptr_type item = force_value_cast<ptr_type>(std::forward<Func>(func));
        if (!items.count(item))
        {
            items.insert(item);
            initializationSequence().emplace_back(std::forward<Func>(func));
        }
        return 0;
    }

    template<typename Func>
    inline byte Engine::subscribeToShutdown(Func&& func)
    {
        static_assert(sizeof(func) == sizeof(ptr_type), "Parameter passed to subscribeToShutdown is not a function.");
        static std::unordered_set<ptr_type> items;

        ptr_type item = force_value_cast<ptr_type>(std::forward<Func>(func));
        if (!items.count(item))
        {
            items.insert(item);
            shutdownSequence().emplace(shutdownSequence().begin(), func);
        }
        return 0;
    }

    template<typename SubSystem>
    inline byte Engine::reportSubSystem()
    {
        subscribeToInit(&SubSystem::init);
        subscribeToShutdown(&SubSystem::shutdown);
        return 0;
    }

    template <typename ModuleType, typename... Args CNDOXY(inherits_from<ModuleType, Module>)>
    inline void Engine::reportModule(Args && ...args)
    {
        std::unique_ptr<Module> module = std::make_unique<ModuleType>(std::forward<Args>(args)...);
        m_modules[module->priority()].emplace_back(std::move(module));
    }
}
