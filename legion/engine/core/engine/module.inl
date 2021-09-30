#include <core/engine/module.hpp>
#pragma once
#include <core/ecs/registry.hpp>
#include <core/scheduling/scheduling.hpp>
#include <core/engine/system.hpp>

namespace legion::core
{
    template<size_type charc>
    inline L_ALWAYS_INLINE void Module::createProcessChain(const char(&name)[charc]) const
    {
        schd::Scheduler::createProcessChain<charc>(name);
    }


    template<typename SystemType, typename... Args>
    inline void Module::reportSystem(Args&&... args)
    {
        static_assert(std::is_base_of_v<System<SystemType>, SystemType>, "All systems must inherit from System<SystemType>");

        SystemType* system = static_cast<SystemType*>(m_systems.emplace(make_hash<SystemType>(), std::make_unique<SystemType>(std::forward<Args>(args)...)).first->second.get());

        if constexpr (has_setup_v<SystemType, void()>)
        {
            m_setupFuncs.insert_back<SystemType, &SystemType::setup>(system);
        }

        if constexpr (has_shutdown_v<SystemType, void()>)
        {
            m_shutdownFuncs.insert_back<SystemType, &SystemType::shutdown>(system);
        }

        if constexpr (has_update_v<SystemType, void(time::span)>)
        {
            system->template createProcess<&SystemType::update>("Update");
        }
    }

    template<typename component_type, typename... Args>
    inline L_ALWAYS_INLINE void Module::registerComponentType(Args&&... args)
    {
        ecs::Registry::registerComponentType<component_type>(std::forward<Args>(args)...);
    }
}
