#include <core/scheduling/clock.hpp>

#include <thread>

namespace legion::core::scheduling
{
    Clock::span_type Clock::m_lastTickStart = 0;
    advancement_protocol Clock::m_protocol = advancement_protocol::Free;
    Clock::span_type Clock::m_interval = static_cast<Clock::time_type>(1.0 / 60.0);
    Clock::span_type Clock::m_lastTickDuration = 0;
    Clock::span_type Clock::m_waitBuffer = 0;
    std::atomic<bool> Clock::m_doTick = { false };
    multicast_delegate<Clock::tick_callback_type> Clock::m_onTick;
    Clock::time_type Clock::m_timeScale = static_cast<Clock::time_type>(1);

    void Clock::advance(span_type start, span_type elapsed)
    {
        m_lastTickDuration = elapsed;
        m_lastTickStart = start;
        m_onTick(elapsed);
    }

    Clock::time_type Clock::timeScale() noexcept
    {
        return m_timeScale;
    }

    void Clock::setTimeScale(time_type value) noexcept
    {
        m_timeScale = value;
    }

    Clock::span_type Clock::elapsedSinceTickStart() noexcept
    {
        return (time::mainClock.now() - m_lastTickStart) * m_timeScale;
    }

    Clock::span_type Clock::lastTickDuration() noexcept
    {
        return m_lastTickDuration * m_timeScale;
    }

    Clock::span_type Clock::unscaledElapsedSinceTickStart() noexcept
    {
        return time::mainClock.now() - m_lastTickStart;
    }

    Clock::span_type Clock::unscaledLastTickDuration() noexcept
    {
        return m_lastTickDuration;
    }

    void Clock::subscribeToTick(const tick_callback_delegate& func)
    {
        m_onTick.push_back(func);
    }

    void Clock::unsubscribeFromTick(const tick_callback_delegate& func)
    {
        m_onTick.erase(func);
    }

    void Clock::setAdvancementProtocol(advancement_protocol protocol) noexcept
    {
        m_protocol = protocol;
    }

    void Clock::setTickSpeed(span_type interval) noexcept
    {
        m_interval = interval;
    }

    void Clock::update()
    {
        auto loopStart = time::mainClock.now();
        auto elapsedSinceLastTick = loopStart - m_lastTickStart;

        if (m_waitBuffer.nanoseconds() > static_cast<span_type::time_type>(0))
        {
            m_waitBuffer -= elapsedSinceLastTick;
            m_lastTickStart = loopStart;
            return;
        }

        switch (m_protocol)
        {
        case advancement_protocol::Free:
            advance(loopStart, elapsedSinceLastTick);
            break;
        case advancement_protocol::Interval:
            if (elapsedSinceLastTick >= m_interval)
                advance(loopStart, elapsedSinceLastTick);
            break;
        case advancement_protocol::Manual:
            if (m_doTick.exchange(false, std::memory_order_acquire))
                advance(loopStart, elapsedSinceLastTick);
            else
                std::this_thread::sleep_for(std::chrono::microseconds(1));
            break;
        }
    }

    void Clock::pause(span_type duration) noexcept
    {
        m_waitBuffer = duration;
    }

    void Clock::bufferPause(span_type duration) noexcept
    {
        m_waitBuffer += duration;
    }

    void Clock::tick()
    {
        m_doTick.store(true, std::memory_order_release);
    }
}
