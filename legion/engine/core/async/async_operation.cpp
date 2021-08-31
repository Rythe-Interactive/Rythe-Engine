#include <core/async/async_operation.hpp>
#include <thread>
#include <core/logging/logging.hpp>

namespace legion::core::async
{
    float async_progress_base::size() const noexcept
    {
        return static_cast<float>(m_size) / precision_scale<float>;
    }

    size_type async_progress_base::raw_size() const noexcept
    {
        return m_size;
    }

    size_type async_progress_base::raw_progress() const noexcept
    {
        return m_progress.load(std::memory_order_relaxed);
    }

    void async_progress_base::complete_impl() noexcept
    {
        m_progress.store(m_size, std::memory_order_release);
    }

    void async_progress_base::advance_progress(float progress) noexcept
    {
        m_progress.fetch_add(static_cast<size_type>(progress * precision_scale<float>), std::memory_order_release);
    }

    void async_progress_base::reset(float progress) noexcept
    {
        m_progress.store(static_cast<size_type>(progress * precision_scale<float>), std::memory_order_release);
    }

    bool async_progress_base::is_done() const noexcept
    {
        return m_progress.load(std::memory_order_relaxed) >= m_size;
    }

    float async_progress_base::progress() const noexcept
    {
        return static_cast<float>(m_progress.load(std::memory_order_relaxed)) / static_cast<float>(m_size);
    }

    async_operation_base::async_operation_base(const std::shared_ptr<async_progress_base>& progress) noexcept
        : m_progress(progress)
    {
    }

    bool async_operation_base::is_done() const noexcept
    {
        return m_progress->is_done();
    }

    float async_operation_base::progress() const noexcept
    {
        return m_progress->progress();
    }

    void async_operation_base::wait(wait_priority priority) const noexcept
    {
        OPTICK_EVENT("legion::core::async::async_operation<T>::wait");
        while (!m_progress->is_done())
        {
            switch (priority)
            {
            case wait_priority::sleep:
                std::this_thread::sleep_for(std::chrono::microseconds(1));
                break;
            case wait_priority::normal:
                std::this_thread::yield();
                break;
            case wait_priority::real_time:
            default:
                L_PAUSE_INSTRUCTION();
                break;
            }
        }
    }

}
