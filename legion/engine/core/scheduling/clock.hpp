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
        using tick_callback_type = void(span_type);
        using tick_callback_delegate = delegate<tick_callback_type>;

    private:
        static span_type m_lastTickStart;
        static advancement_protocol m_protocol;
        static span_type m_interval;
        static span_type m_lastTickDuration;
        static span_type m_waitBuffer;
        static std::atomic<bool> m_stop;
        static std::atomic<bool> m_doTick;
        static multicast_delegate<tick_callback_type> m_onTick;

        void advance(span_type start, span_type elapsed);

    public:
        span_type elapsedSinceTickStart() const noexcept;
        span_type lastTickDuration() const noexcept;

        void subscribeToTick(const tick_callback_delegate& func);
        void unsubscribeFromTick(const tick_callback_delegate& func);

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
