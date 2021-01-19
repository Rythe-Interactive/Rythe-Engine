#pragma once
#include <core/platform/platform.hpp>
#include <core/types/types.hpp>
#include <core/async/wait_priority.hpp>
#include <atomic>
#include <memory>
#include <thread>

namespace legion::core::async
{
    struct async_progress
    {
    protected:
        size_type m_size;
        std::atomic<size_type> m_progress;

    public:
        async_progress(size_type size) : m_size(size), m_progress(0) {}

        size_type size() const noexcept;
        size_type rawProgress() const noexcept;

        void complete() noexcept;
        void advanceProgress(size_type progress) noexcept;
        bool isDone() const noexcept;
        float progress() const noexcept;
    };

    template<typename Func>
    struct async_operation
    {
    protected:
        std::shared_ptr<async_progress> m_progress;
        Func m_repeater;
    public:
        explicit async_operation(const std::shared_ptr<async_progress>& progress, const Func& repeater) : m_progress(progress), m_repeater(repeater) {}
        async_operation() = default;
        async_operation(const async_operation&) = default;
        async_operation(async_operation&&) = default;

        bool isDone() const noexcept
        {
            return m_progress->isDone();
        }

        float progress() const noexcept
        {
            return m_progress->progress();
        }

        virtual void wait(wait_priority priority = wait_priority_normal) const noexcept
        {
            while (!m_progress->isDone())
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

        template<typename... Args>
        auto then(Args&&... args) const
        {
            wait();
            return m_repeater(std::forward<Args>(args)...);
        }

        template<typename... Args>
        auto then(wait_priority priority, Args&&... args) const
        {
            wait(priority);
            return m_repeater(std::forward<Args>(args)...);
        }
    };

    template<typename Func>
    async_operation(const std::shared_ptr<async_progress>&, const Func&)->async_operation<Func>;
}
