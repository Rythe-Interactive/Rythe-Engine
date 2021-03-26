#include <core/async/job_pool.hpp>

namespace legion::core::async
{
    thread_local id_type this_job::m_id;
    thread_local float this_job::m_progress;

    id_type this_job::get_id() noexcept
    {
        return m_id;
    }

    float this_job::get_progress() noexcept
    {
        return m_progress;
    }

    std::shared_ptr<async_progress> job_pool::get_progress() const noexcept
    {
        return m_progress;
    }

    bool job_pool::empty() const noexcept
    {
        size_type idx = m_index.load(std::memory_order_relaxed);
        return idx < 1 || idx > m_size;
    }

    void job_pool::complete_job()
    {
        OPTICK_EVENT();
        size_type idx = m_index.fetch_sub(1, std::memory_order_acquire);
        if (idx < 1 || idx > m_size)
            return;

        size_type id = m_size - idx;
        this_job::m_id = id;
        this_job::m_progress = m_progress->progress();
        m_job();
        m_progress->advance_progress();
    }

    bool job_pool::is_done() const noexcept
    {
        return m_progress->is_done();
    }

}
