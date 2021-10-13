#pragma once
#include <list>
#include <memory>

#include <Optick/optick.h>

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
        static thread_local float m_progress;
    public:
        static id_type get_id() noexcept;
        static float get_progress() noexcept;
    };

    struct job_pool
    {
    private:
        std::shared_ptr<async_progress<void>> m_progress;
        std::atomic<size_type> m_index;
        size_type m_size;
        delegate<void()> m_job;

    public:
        job_pool(size_type count, const delegate<void()>& func) : m_progress(new async_progress<void>(count)), m_index(count), m_size(count), m_job(func) {}

        std::shared_ptr<async_progress<void>> get_progress() const noexcept;

        bool empty() const noexcept;

        bool prime_job();
        void complete_job();

        bool is_done() const noexcept;
    };

    using job_queue = std::list<std::shared_ptr<job_pool>>;

    template<typename Func, typename CompleteFunc>
    struct job_operation : public repeating_async_operation<Func, void>
    {
    private:
        CompleteFunc m_onComplete;

    public:
        std::shared_ptr<job_pool> jobPoolPtr;

        job_operation(const std::shared_ptr<async_progress<void>>& progress, const std::shared_ptr<job_pool>& jobPool, Func&& repeater, CompleteFunc&& complete)
            : repeating_async_operation<Func, void>(progress, repeater), m_onComplete(complete), jobPoolPtr(jobPool) {}

        job_operation(const job_operation&) noexcept(std::is_nothrow_copy_constructible_v<Func> && std::is_nothrow_copy_constructible_v<CompleteFunc>) = default;
        job_operation(job_operation&&) noexcept(std::is_nothrow_move_constructible_v<Func> && std::is_nothrow_move_constructible_v<CompleteFunc>) = default;

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
                    jobPoolPtr->prime_job();
                    jobPoolPtr->complete_job();
                    L_PAUSE_INSTRUCTION();
                    break;
                }
                case wait_priority::real_time:
                default:
                {
                    jobPoolPtr->prime_job();
                    jobPoolPtr->complete_job();
                    break;
                }
                }
            }
            m_onComplete();
        }
    };

#if !defined(DOXY_EXCLUDE)
    template<typename Func, typename CompletionFunc>
    job_operation(
        const std::shared_ptr<async_progress<void>>&,
        const std::shared_ptr<job_pool>&,
        Func&&, CompletionFunc&&)->job_operation<Func, CompletionFunc>;
#endif
}
