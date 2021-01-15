#include <core/async/async_operation.hpp>
#include <thread>
#include <core/logging/logging.hpp>

namespace legion::core::async
{
    size_type async_progress::size() const noexcept
    {
        return m_size;
    }

    size_type async_progress::rawProgress() const noexcept
    {
        return m_progress.load(std::memory_order_relaxed);
    }

    void async_progress::complete() noexcept
    {
        m_progress.store(m_size, std::memory_order_release);
    }

    void async_progress::advanceProgress(size_type progress) noexcept
    {
        m_progress.fetch_add(progress, std::memory_order_release);
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
