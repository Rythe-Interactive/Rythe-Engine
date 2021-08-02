#pragma once
#include <queue>
#include <memory>

#include <Optick/optick.h>

#include <core/containers/delegate.hpp>

#include <core/async/async_operation.hpp>

namespace legion::core::async
{
    struct async_runnable_base
    {
        std::shared_ptr<async_progress> m_progress;

        async_runnable_base() = default;
        async_runnable_base(float taskSize) : m_progress(new async_progress(taskSize)) {}

        const std::shared_ptr<async_progress>& getProgress() const noexcept
        {
            return m_progress;
        }

        template<typename functor>
        repeating_async_operation<functor> getRepeatingOperation(functor&& then) const noexcept
        {
            return repeating_async_operation<functor>(m_progress, std::forward<functor>(then));
        }

        async_operation getOperation() const noexcept
        {
            return async_operation(m_progress);
        }

        virtual void execute() LEGION_PURE;

        virtual ~async_runnable_base() = default;
    };

    template<typename functor>
    struct async_runnable : public async_runnable_base
    {
    protected:
        functor m_func;

    public:
        async_runnable() = default;
        async_runnable(functor&& func) : async_runnable_base(1), m_func(func) {}
        async_runnable(functor&& func, size_type taskSize) : async_runnable_base(taskSize), m_func(func) {}

        void execute()
        {
            OPTICK_EVENT();
            if constexpr (std::is_invocable_v<functor, async::async_progress&>)
            {
                std::invoke(m_func, *m_progress);
            }
            else
            {
                std::invoke(m_func);
            }

            m_progress->complete();
        }
    };

    using runnables_queue = std::queue<std::unique_ptr<async_runnable_base>>;

#if !defined(DOXY_EXCLUDE)
    template<typename functor>
    async_runnable(functor&&)->async_runnable<functor>;

    template<typename functor>
    async_runnable(functor&&, size_type)->async_runnable<functor>;
#endif

}

