#pragma once
#include <core/async/async_operation.hpp>
#include <core/containers/runnable.hpp>

namespace legion::core::async
{
    template<typename Func>
    struct async_runnable : public runnable<Func>
    {
    protected:
        std::shared_ptr<async_progress> m_progress;
    public:
        async_runnable() = default;
        async_runnable(const Func& func) : runnable<Func>(func), m_progress(new async_progress(1)) {}

        std::shared_ptr<async_progress> getProgress() noexcept
        {
            return m_progress;
        }

        template<typename RepeaterFunc>
        async_operation<RepeaterFunc> getOperation(const RepeaterFunc& func) noexcept
        {
            return async_operation<RepeaterFunc>(m_progress, func);
        }

        virtual void execute() override
        {
            this->m_func();
            m_progress->complete();
        }
    };
}

