#include <core/async/spinlock.hpp>
#include <Optick/optick.h>

namespace legion::core::async
{
    bool spinlock::m_forceRelease = false;
    std::atomic_uint spinlock::m_lastId = { 1 };
    thread_local std::unordered_map<id_type, uint> spinlock::m_localState;

    void spinlock::force_release(bool release)
    {
        m_forceRelease = release;
    }

    spinlock::spinlock(spinlock&& source) noexcept
    {
        if (m_forceRelease)
            return;

        assert_msg("Attempted to move a spinlock that was locked.", !source.m_lock.load(std::memory_order_relaxed));
        m_id = source.m_id;
    }

    spinlock& spinlock::operator=(spinlock&& source) noexcept
    {
        if (m_forceRelease)
            return *this;

        assert_msg("Attempted to move a spinlock that was locked.", !source.m_lock.load(std::memory_order_relaxed));
        m_id = source.m_id;
        return *this;
    }

    void spinlock::lock() const noexcept
    {
        OPTICK_EVENT();
        if (m_forceRelease)
            return;

        auto& locks = m_localState[m_id];
        if (locks)
        {
            locks++;
            return;
        }

        while (true)
        {
            OPTICK_CATEGORY("Acquire spinlock", Optick::Category::Wait);
            if (!m_lock.exchange(true, std::memory_order_acquire))
                break;
            while (m_lock.load(std::memory_order_relaxed))
                L_PAUSE_INSTRUCTION();
        }

        locks++;
    }

    L_NODISCARD bool spinlock::try_lock() const noexcept
    {
        OPTICK_EVENT();
        if (m_forceRelease)
            return true;

        auto& locks = m_localState[m_id];
        if (locks)
        {
            locks++;
            return true;
        }

        bool ret = !m_lock.load(std::memory_order_relaxed) && !m_lock.exchange(true, std::memory_order_acquire);

        if (ret)
            locks++;

        return ret;
    }

    void spinlock::unlock() const noexcept
    {
        OPTICK_EVENT();
        if (m_forceRelease)
            return;

        m_lock.store(false, std::memory_order_release);
        m_localState[m_id]--;
    }
}
