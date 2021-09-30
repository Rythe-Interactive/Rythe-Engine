#include <core/engine/enginesubsystem.hpp>
#pragma once

namespace legion::core
{
    template<class SubSystem>
    bool EngineSubSystem<SubSystem>::m_isInitialized;

    template<class SubSystem>
    bool EngineSubSystem<SubSystem>::m_isShutdown;

    template<class SubSystem>
    typename EngineSubSystem<SubSystem>::data EngineSubSystem<SubSystem>::m_data;

    template<class SubSystem>
    SubSystem& EngineSubSystem<SubSystem>::instance = EngineSubSystem<SubSystem>::m_data.instance;

    template<class SubSystem>
    template<typename... Args>
    inline L_ALWAYS_INLINE SubSystem& EngineSubSystem<SubSystem>::create(Args&&... args)
    {
        new(&m_data.instance) SubSystem(std::forward<Args>(args)...);
        return instance;
    }

    template<class SubSystem>
    inline L_ALWAYS_INLINE SubSystem& EngineSubSystem<SubSystem>::getInstance()
    {
        init();
        return instance;
    }

    template<class SubSystem>
    inline L_ALWAYS_INLINE bool EngineSubSystem<SubSystem>::initialized()
    {
        return m_isInitialized;
    }


    template<class SubSystem>
    inline void EngineSubSystem<SubSystem>::init()
    {
        if (!m_isInitialized)
        {
            m_isShutdown = false;
            m_isInitialized = true;

            if constexpr (has_static_onInit_v<SubSystem, void()>)
                SubSystem::onInit();
            else
            {
                static_assert(std::is_default_constructible_v<SubSystem>, "Sub-System is not default constructible and hasn't provided an OnInit function to take care of construction.");
                create();
            }
        }
    }

    template<class SubSystem>
    inline void EngineSubSystem<SubSystem>::shutdown()
    {
        if (!m_isShutdown && m_isInitialized)
        {
            m_isShutdown = true;
            m_isInitialized = false;

            if constexpr (has_static_onShutdown_v<SubSystem, void()>)
                SubSystem::onShutdown();

            instance.~SubSystem();
        }
    }

    template<class SubSystem>
    inline L_ALWAYS_INLINE void EngineSubSystem<SubSystem>::restart()
    {
        shutDown();
        init();
    }
}
