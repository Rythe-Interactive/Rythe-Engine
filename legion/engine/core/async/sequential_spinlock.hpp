#pragma once
#include <core/platform/platform.hpp>
#include <atomic>

namespace legion::core::async
{
    struct sequential_spinlock final
    {
    private:
        std::atomic_bool m_lock = { false };

    public:

        sequential_spinlock() = default;
        sequential_spinlock(const sequential_spinlock&) = delete;
        sequential_spinlock& operator=(const sequential_spinlock&) = delete;

        void lock() noexcept
        {
            while (true)
            {
                if (!m_lock.exchange(true, std::memory_order_acquire))
                    break;
                while (m_lock.load(std::memory_order_relaxed))
                    L_PAUSE_INSTRUCTION();
            }
        }

        L_NODISCARD bool try_lock() noexcept
        {
            return !m_lock.load(std::memory_order_release) && !m_lock.exchange(true, std::memory_order_acquire);
        }

        void unlock()
        {
            m_lock.store(false, std::memory_order_release);
        }
    };
}
