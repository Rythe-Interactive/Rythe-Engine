#include <core/async/async_operation.hpp>
#include <thread>
#include <core/logging/logging.hpp>

namespace legion::core::async
{
    void async_progress::complete() noexcept
    {
        m_progress.store(m_size, std::memory_order_release);
    }

    void async_progress::setProgress(size_type progress) noexcept
    {
        size_type currentProgress = m_progress.load(std::memory_order_relaxed);
        while (!m_progress.compare_exchange_weak(currentProgress, progress, std::memory_order_release, std::memory_order_relaxed))
            if (currentProgress >= progress)
                break;
    }

    bool async_progress::isDone() const noexcept
    {
        return m_progress.load(std::memory_order_relaxed) >= m_size;
    }

    float async_progress::progress() const noexcept
    {
        return ((float)m_progress.load(std::memory_order_relaxed)) / m_size;
    }

}
