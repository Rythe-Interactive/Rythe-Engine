#pragma once
#include <atomic>

#include <core/containers/delegate.hpp>
#include <core/time/time.hpp>

namespace legion::core::scheduling
{
    enum struct advancement_protocol
    {
        Free, Interval, Manual
    };

    class Clock
    {
    public:
        using span_type = decltype(time::mainClock)::span_type;

    private:
        static span_type m_lastTickStart;
        static advancement_protocol m_protocol;
        static span_type m_interval;
        static span_type m_lastTickDuration;
        static span_type m_waitBuffer;
        static std::atomic<bool> m_stop;
        static std::atomic<bool> m_doTick;
        static multicast_delegate<void(span_type)> m_onTick;

        void advance(span_type start, span_type elapsed);

    public:
        template <class owner_type, void(owner_type::* func_type)(span_type)>
        static void subscribeToTick(owner_type* instance)
        {
            m_onTick += delegate<void(span_type)>::create<owner_type, func_type>(instance);
        }

        template <class owner_type, void(owner_type::* func_type)(span_type) const>
        static void subscribeToTick(owner_type const* instance)
        {
            m_onTick += delegate<void(span_type)>::create<owner_type, func_type>(instance);
        }

        template <void(*func_type)(span_type)>
        static void subscribeToTick()
        {
            m_onTick += delegate<void(span_type)>::create<func_type>();
        }

        template <typename lambda_type>
        static void subscribeToTick(const lambda_type& instance)
        {
            m_onTick += delegate<void(span_type)>::create<lambda_type>(instance);
        }

        void setAdvancementProtocol(advancement_protocol protocol) noexcept;

        void setTickSpeed(span_type interval) noexcept;

        void run();

        void pause(span_type duration) noexcept;

        void bufferPause(span_type duration) noexcept;

        void stop() noexcept;

        void tick();

        span_type tickDuration() noexcept;
    };
}
