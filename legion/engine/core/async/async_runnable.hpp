#pragma once

#include <core/containers/delegate.hpp>

#include <core/async/async_operation.hpp>

namespace legion::core::async
{
    struct async_runnable_base
    {
        std::shared_ptr<async_progress> m_progress;

        async_runnable_base() = default;
        async_runnable_base(size_type taskSize) : m_progress(new async_progress(taskSize)) {}

        std::shared_ptr<async_progress> getProgress() noexcept
        {
            return m_progress;
        }

        template<typename functor>
        async_operation<functor> getOperation(functor&& then) noexcept
        {
            return async_operation<functor>(m_progress, std::forward<functor>(then));
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

#if !defined(DOXY_EXCLUDE)
    template<typename functor>
    async_runnable(functor&&)->async_runnable<functor>;

    template<typename functor>
    async_runnable(functor&&, size_type)->async_runnable<functor>;
#endif

}

