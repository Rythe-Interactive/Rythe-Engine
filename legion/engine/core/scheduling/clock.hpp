#pragma once
#include <atomic>

#include <core/engine/engine.hpp>
#include <core/engine/enginesubsystem.hpp>
#include <core/containers/delegate.hpp>
#include <core/time/time.hpp>

namespace legion::core::scheduling
{
    enum struct advancement_protocol
    {
        Free, Interval, Manual
    };

    class Clock : public EngineSubSystem<Clock>
    {
        AllowPrivateOnInit;
        SubSystemInstance(Clock);
    public:
        using span_type = time::main_clock::span_type;
        using time_type = span_type::time_type;
        using tick_callback_type = void(span_type);
        using tick_callback_delegate = delegate<tick_callback_type>;

    private:
        span_type m_lastTickStart = 0;
        advancement_protocol m_protocol = advancement_protocol::Free;
        span_type m_interval = static_cast<Clock::time_type>(1.0 / 60.0);
        span_type m_lastTickDuration = 0;
        span_type m_waitBuffer = 0;
        std::atomic<bool> m_doTick = { false };
        multicast_delegate<tick_callback_type> m_onTick;
        time_type m_timeScale = static_cast<Clock::time_type>(1);

        static void advance(span_type start, span_type elapsed);

        static void onInit();

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

    ReportSubSystem(Clock);
}
