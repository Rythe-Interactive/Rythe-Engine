#include <core/engine/enginesubsystem.hpp>
#pragma once

namespace legion::core
{
    template<class SubSystem>
    bool EngineSubSystem<SubSystem>::m_isInitialized;

    template<class SubSystem>
    bool EngineSubSystem<SubSystem>::m_isShutdown;

    template<class SubSystem>
    bool EngineSubSystem<SubSystem>::m_constructed;

    template<class SubSystem>
    typename EngineSubSystem<SubSystem>::data EngineSubSystem<SubSystem>::m_data;

    template<class SubSystem>
    template<typename... Args>
    inline L_ALWAYS_INLINE SubSystem& EngineSubSystem<SubSystem>::create(Args&&... args)
    {
        new(&m_data.inst) SubSystem(std::forward<Args>(args)...);
        m_constructed = true;
        return m_data.inst;
    }

    template<class SubSystem>
    template<typename T>
    inline void EngineSubSystem<SubSystem>::reportDependency()
    {
        static_assert(std::is_base_of_v<EngineSubSystem<T>, T>, "Dependency must be an EngineSubSystem.");
        auto& shutdownSeq = T::shutdownSequence();
        shutdownSeq.emplace_back(&SubSystem::shutdown);

        T::init();
    }

    template<class SubSystem>
    inline multicast_delegate<void()>& EngineSubSystem<SubSystem>::shutdownSequence()
    {
        static multicast_delegate<void()> m_shutdownSequence;
        return m_shutdownSequence;
    }

    template<class SubSystem>
    inline L_ALWAYS_INLINE SubSystem& EngineSubSystem<SubSystem>::getInstance()
    {
        return m_data.inst;
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
            log::setup();
            m_isShutdown = false;
            m_isInitialized = true;

            log::undecoratedInfo("== Initializing {}", nameOfType<SubSystem>());

            if constexpr (has_static_onInit_v<SubSystem, void()>)
            {
                SubSystem::onInit();

                if (!m_constructed)
                {
                    if constexpr (std::is_default_constructible_v<SubSystem>)
                    {
                        log::warn("Subsystem {} does not create instance in onInit() and is default constructed.", nameOfType<SubSystem>());
                        create();
                    }
                    else
                    {
                        log::error("Subsystem {} does not create instance in onInit() and is not default constructable.", nameOfType<SubSystem>());
                        throw legion_exception_msg("Subsystem does not create instance in onInit() and is not default constructable.");
                    }
                }
            }
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

            shutdownSequence()();

            log::undecoratedInfo("== Shutting down {}", nameOfType<SubSystem>());

            if constexpr (has_static_onShutdown_v<SubSystem, void()>)
                SubSystem::onShutdown();

            m_data.inst.~SubSystem();
        }
    }

    template<class SubSystem>
    inline L_ALWAYS_INLINE void EngineSubSystem<SubSystem>::restart()
    {
        shutdown();
        init();
    }
}
