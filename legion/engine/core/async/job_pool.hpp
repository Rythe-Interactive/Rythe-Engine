#pragma once
#include <queue>
#include <memory>

#include <core/async/async_operation.hpp>
#include <core/containers/delegate.hpp>
#include <core/containers/pointer.hpp>

namespace legion::core::async
{
    struct this_job
    {
        friend struct job_pool;
    private:
        static thread_local id_type m_id;
    public:
        static id_type get_id() noexcept;
    };

    struct job_pool
    {
    private:
        std::shared_ptr<async_progress> m_progress;
        std::atomic<size_type> m_index;
        size_type m_size;
        delegate<void()> m_job;

    public:
        job_pool(size_type count, const delegate<void()>& func) : m_progress(new async_progress(count)), m_index(count), m_size(count), m_job(func) {}

        std::shared_ptr<async_progress> get_progress() const noexcept
        {
            return m_progress;
        }

        bool empty() const noexcept
        {
            size_type idx = m_index.load(std::memory_order_relaxed);
            return idx < 1 || idx > m_size;
        }

        void complete_job()
        {
            size_type idx = m_index.fetch_sub(1, std::memory_order_acquire);
            if (idx < 1 || idx > m_size)
                return;

            size_type id = m_size - idx;
            this_job::m_id = id;
            m_job();
            m_progress->advance_progress();
        }

        bool is_done() const noexcept
        {
            return m_progress->is_done();
        }
    };

    using job_queue = std::queue<std::shared_ptr<job_pool>>;

    template<typename Func, typename CompleteFunc>
    struct job_operation : public async_operation<Func>
    {
    private:
        CompleteFunc m_onComplete;

        void execute_job() const noexcept
        {
            OPTICK_EVENT();

            jobPoolPtr->complete_job();

            if (jobPoolPtr->is_done())
            {
                m_onComplete();
            }
        }

    public:
        std::shared_ptr<job_pool> jobPoolPtr;

        job_operation(const std::shared_ptr<async_progress>& progress, const std::shared_ptr<job_pool>& jobPool, const Func& repeater, const CompleteFunc& complete)
            : async_operation<Func>(progress, repeater), m_onComplete(complete), jobPoolPtr(jobPool) {}
        job_operation(const job_operation&) = default;
        job_operation(job_operation&&) = default;

        virtual void wait(wait_priority priority = wait_priority_normal) const noexcept override
        {
            if (!jobPoolPtr)
                return;

            OPTICK_EVENT("legion::core::async::job_operation<T>::wait");
            while (!jobPoolPtr->is_done())
            {
                switch (priority)
                {
                case wait_priority::sleep:
                    std::this_thread::sleep_for(std::chrono::microseconds(1));
                    break;
                case wait_priority::normal:
                {
                    execute_job();
                    L_PAUSE_INSTRUCTION();
                    break;
                }
                case wait_priority::real_time:
                default:
                {
                    execute_job();
                    break;
                }
                }
            }
        }
    };

#if !defined(DOXY_EXCLUDE)
    template<typename Func, typename CompletionFunc>
    job_operation(
        const std::shared_ptr<async_progress>&,
        const std::shared_ptr<job_pool>&,
        const Func&, const CompletionFunc&)->job_operation<Func, CompletionFunc>;
#endif
}
