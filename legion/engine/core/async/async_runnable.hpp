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
        std::shared_ptr<async_progress_base> m_progress;

        async_runnable_base() = default;
        async_runnable_base(const std::shared_ptr<async_progress_base>& progress) : m_progress(progress) {}

        virtual void execute() LEGION_PURE;

        virtual ~async_runnable_base() = default;
    };

    template<typename functor>
    struct async_runnable : public async_runnable_base
    {
        using payload_type = std::conditional_t<std::is_invocable_v<functor, async::async_progress_base&>,
                                    decltype(std::invoke(std::declval<functor>(), std::declval<async::async_progress_base&>())),
                                    decltype(std::invoke(std::declval<functor>()))>;
    protected:
        functor m_func;

    public:
        async_runnable() = default;
        async_runnable(functor&& func) : async_runnable_base(std::make_shared<async_progress<payload_type>>(1)), m_func(func) {}
        async_runnable(functor&& func, size_type taskSize) : async_runnable_base(std::make_shared<async_progress<payload_type>>(taskSize)), m_func(func) {}

        const std::shared_ptr<async_progress<payload_type>>& getProgress() const noexcept
        {
            return std::static_pointer_cast<async_progress<payload_type>>(m_progress);
        }

        template<typename func>
        repeating_async_operation<func, payload_type> getRepeatingOperation(func&& then) const noexcept
        {
            return repeating_async_operation<func, payload_type>(
                std::static_pointer_cast<async_progress<payload_type>>(m_progress),
                std::forward<func>(then));
        }

        async_operation<payload_type> getOperation() const noexcept
        {
            return async_operation<payload_type>(std::static_pointer_cast<async_progress<payload_type>>(m_progress));
        }

        auto execute()
        {
            OPTICK_EVENT();
            if constexpr (std::is_same_v<payload_type, void>)
            {
                if constexpr (std::is_invocable_v<functor, async_progress_base&>)
                {
                    std::invoke(m_func, *m_progress);
                }
                else
                {
                    std::invoke(m_func);
                }
                std::static_pointer_cast<async_progress<payload_type>>(m_progress)->complete();
            }
            else
            {
                if constexpr (std::is_invocable_v<functor, async_progress_base&>)
                {
                    std::static_pointer_cast<async_progress<payload_type>>(m_progress)->complete(std::invoke(m_func, *m_progress));
                }
                else
                {
                    std::static_pointer_cast<async_progress<payload_type>>(m_progress)->complete(std::invoke(m_func));
                }
            }
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

