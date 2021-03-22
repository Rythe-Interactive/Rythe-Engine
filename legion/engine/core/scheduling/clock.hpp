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
        void subscribeToTick(delegate<void(span_type)>& func);
        void unsubscribeFromTick(delegate<void(span_type)>& func);

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
