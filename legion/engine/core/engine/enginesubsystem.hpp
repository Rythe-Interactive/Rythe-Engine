#pragma once
#include <core/platform/platform.hpp>
#include <core/types/meta.hpp>

namespace legion::core
{
    HAS_FUNC(onInit);
    HAS_FUNC(onShutdown);

    template<class SubSystem>
    class EngineSubSystem
    {
    private:
        static bool m_isInitialized;
        static bool m_isShutdown;

        struct data
        {
            union
            {
                SubSystem instance;
                byte dummy;
            };

            data() : dummy(0) {}
            ~data() {}
        };

        static data m_data;

    protected:

        static SubSystem& instance;

        template<typename... Args>
        static SubSystem& create(Args&&... args);

    public:
        L_NODISCARD static SubSystem& getInstance();

        L_NODISCARD static bool initialized();

        static void init();

        static void shutdown();

        static void restart();
    };
}

#define ALLOW_PRIVATE_ONINIT                        \
template<typename, typename>                        \
friend struct legion::core::has_static_onInit;      \
template<typename>                                  \
friend class legion::core::EngineSubSystem;

#define ALLOW_PRIVATE_ONSHUTDOWN                    \
template<typename, typename>                        \
friend struct legion::core::has_static_onShutdown;  \
template<typename>                                  \
friend class legion::core::EngineSubSystem;

#include <core/engine/enginesubsystem.inl>
