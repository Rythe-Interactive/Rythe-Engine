#pragma once
#include <core/async/async_operation.hpp>
#include <core/containers/runnable.hpp>

namespace legion::core::async
{
    template<typename Func>
    struct job_pool;

    struct this_job
    {
        template<typename T>
        friend struct job_pool;
    private:
        static thread_local id_type m_id;
    public:
        static id_type get_id() noexcept;
    };

    struct job_pool_base
    {
    protected:
        std::shared_ptr<async_progress> m_progress;
    public:
        job_pool_base(size_type count) : m_progress(new async_progress(count)) {}

        std::shared_ptr<async_progress> getProgress() const noexcept
        {
            return m_progress;
        }

        void complete() noexcept { m_progress->complete(); }

        virtual runnable_base* pop_job() LEGION_PURE;
        virtual bool empty() const noexcept LEGION_PURE;
    };

    template<typename Func>
    struct job_operation : public async_operation<Func>
    {
    protected:
        job_pool_base* m_jobPool;
    public:
        job_operation(const std::shared_ptr<async_progress>& progress, job_pool_base* jobPool, const Func& repeater)
            : async_operation<Func>(progress, repeater), m_jobPool(jobPool) {}
        job_operation(const job_operation&) = default;
        job_operation(job_operation&&) = default;

        virtual void wait(wait_priority priority = wait_priority_normal) const noexcept override
        {
            while (!m_progress->isDone())
            {
                switch (priority)
                {
                case wait_priority::sleep:
                    std::this_thread::sleep_for(std::chrono::microseconds(1));
                    break;
                case wait_priority::normal:
                {
                    auto* job = m_jobPool->pop_job();
                    if (job)
                        job->execute();
                    else
                        m_progress->complete();
                    L_PAUSE_INSTRUCTION();
                    break;
                }
                case wait_priority::real_time:
                default:
                {
                    auto* job = m_jobPool->pop_job();
                    if (job)
                        job->execute();
                    else
                        m_progress->complete();
                    break;
                }
                }
            }
        }
    };

    template<typename Func>
    job_operation(const std::shared_ptr<async_progress>&, job_pool_base* jobPool, const Func&)->job_operation<Func>;

    template<typename Func>
    struct job_pool : public job_pool_base
    {
    private:
        std::atomic<size_type> m_index;
        std::vector<runnable<Func>> m_jobs;

    public:
        job_pool(size_type count, const Func& func) : job_pool_base(count), m_index(count)
        {
            m_jobs.resize(count, runnable<Func>(func));
        }

        template<typename RepeaterFunc>
        job_operation<RepeaterFunc> getOperation(const RepeaterFunc& func)
        {
            return job_operation<RepeaterFunc>(getProgress(), this, func);
        }

        virtual runnable_base* pop_job() override
        {
            size_type idx = m_index.fetch_sub(1, std::memory_order_acquire);
            if (idx < 1 || idx > m_jobs.size())
                return nullptr;

            size_type id = m_jobs.size() - idx;
            this_job::m_id = id;
            m_progress->setProgress(id);
            auto* ret = &m_jobs[idx - 1];
            return ret;
        }

        virtual bool empty() const noexcept override
        {
            size_type idx = m_index.load(std::memory_order_relaxed);
            return idx == 0 || idx > m_jobs.size();
        }
    };
}
