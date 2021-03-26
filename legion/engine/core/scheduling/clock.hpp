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
        using time_type = span_type::time_type;
        using tick_callback_type = void(span_type);
        using tick_callback_delegate = delegate<tick_callback_type>;

    private:
        static span_type m_lastTickStart;
        static advancement_protocol m_protocol;
        static span_type m_interval;
        static span_type m_lastTickDuration;
        static span_type m_waitBuffer;
        static std::atomic<bool> m_doTick;
        static multicast_delegate<tick_callback_type> m_onTick;
        static time_type m_timeScale;

        static void advance(span_type start, span_type elapsed);

    public:
        static time_type timeScale() noexcept;
        static void setTimeScale(time_type value) noexcept;

        static span_type elapsedSinceTickStart() noexcept;
        static span_type lastTickDuration() noexcept;
        static span_type unscaledElapsedSinceTickStart() noexcept;
        static span_type unscaledLastTickDuration() noexcept;

        static void subscribeToTick(const tick_callback_delegate& func);
        static void unsubscribeFromTick(const tick_callback_delegate& func);

        static void setAdvancementProtocol(advancement_protocol protocol) noexcept;

        static void setTickSpeed(span_type interval) noexcept;

        static void update();

        static void pause(span_type duration) noexcept;

        static void bufferPause(span_type duration) noexcept;

        static void tick();
    };
}
