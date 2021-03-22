#include <core/scheduling/clock.hpp>

namespace legion::core::scheduling
{
    void Clock::advance(span_type start, span_type elapsed)
    {
        m_lastTickDuration = elapsed;
        m_lastTickStart = start;
        m_onTick(elapsed);
    }

    void Clock::subscribeToTick(const delegate<void(span_type)>& func)
    {
        m_onTick.push_back(func);
    }

    void Clock::unsubscribeFromTick(const delegate<void(span_type)>& func)
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

    void Clock::run()
    {
        while (!m_stop.load(std::memory_order_relaxed))
        {
            auto loopStart = time::mainClock.now();
            auto elapsedSinceLastTick = loopStart - m_lastTickStart;

            if (m_waitBuffer.nanoseconds() > static_cast<span_type::time_type>(0))
            {
                m_waitBuffer -= elapsedSinceLastTick;
                m_lastTickStart = loopStart;
                continue;
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
                break;
            }
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

    void Clock::stop() noexcept
    {
        m_stop.store(true, std::memory_order_release);
    }

    void Clock::tick()
    {
        m_doTick.store(true, std::memory_order_release);
    }

    Clock::span_type Clock::tickDuration() noexcept
    {
        return m_lastTickDuration;
    }

}
