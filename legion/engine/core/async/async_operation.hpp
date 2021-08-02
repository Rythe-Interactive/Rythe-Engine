#pragma once
#include <atomic>
#include <memory>
#include <thread>

#include <core/platform/platform.hpp>
#include <core/types/types.hpp>
#include <core/async/wait_priority.hpp>
#include <core/containers/delegate.hpp>

namespace legion::core::async
{
    struct async_progress
    {
    protected:
        const size_type m_size;
        std::atomic<size_type> m_progress;

        template<typename T>
        constexpr static T precision_scale = static_cast<T>(1000);

    public:
        constexpr async_progress() noexcept : m_size(100 * precision_scale<size_type>), m_progress(0) {}
        constexpr explicit async_progress(float size) noexcept : m_size(static_cast<size_type>(size) * precision_scale<size_type>), m_progress(0) {}

        float size() const noexcept;
        size_type rawSize() const noexcept;
        size_type rawProgress() const noexcept;

        void complete() noexcept;
        void advance_progress(float progress = 1.f) noexcept;
        bool is_done() const noexcept;
        float progress() const noexcept;
    };

    struct async_operation_base
    {
    protected:
        std::shared_ptr<async_progress> m_progress;

    public:
        explicit async_operation_base(const std::shared_ptr<async_progress>& progress) noexcept;
        async_operation_base() noexcept = default;
        async_operation_base(const async_operation_base&) noexcept = default;
        async_operation_base(async_operation_base&&) noexcept = default;

        bool is_done() const noexcept;

        float progress() const noexcept;

        virtual void wait(wait_priority priority = wait_priority_normal) const noexcept;

        virtual ~async_operation_base() = default;
    };

    template<typename functor>
    struct repeating_async_operation : public async_operation_base
    {
    protected:
        functor m_repeater;

    public:
        repeating_async_operation(const std::shared_ptr<async_progress>& progress, functor&& repeater) : async_operation_base(progress), m_repeater(repeater) {}
        repeating_async_operation() noexcept(std::is_nothrow_default_constructible_v<functor>) = default;
        repeating_async_operation(const repeating_async_operation&) noexcept(std::is_nothrow_copy_constructible_v<functor>) = default;
        repeating_async_operation(repeating_async_operation&&) noexcept(std::is_nothrow_move_constructible_v<functor>) = default;

        template<typename... argument_types>
        auto then(argument_types... args) const
        {
            wait();
            return m_repeater(std::forward<argument_types>(args)...);
        }

        template<typename... argument_types>
        auto then(wait_priority priority, argument_types... args) const
        {
            wait(priority);
            return m_repeater(std::forward<argument_types>(args)...);
        }

        virtual ~repeating_async_operation() = default;
    };

#if !defined(DOXY_EXCLUDE)
    template<typename functor>
    repeating_async_operation(const std::shared_ptr<async_progress>&, functor&&)->repeating_async_operation<functor>;
#endif

    struct async_operation : public async_operation_base
    {
    public:
        async_operation(const std::shared_ptr<async_progress>& progress) : async_operation_base(progress) {}
        async_operation() noexcept = default;
        async_operation(const async_operation&) noexcept = default;
        async_operation(async_operation&&) noexcept = default;

        template<typename Functor, typename... argument_types>
        auto then(Functor&& func, argument_types... args) const
        {
            wait();
            return std::invoke(std::forward<Functor>(func), std::forward<argument_types>(args)...);
        }

        template<typename Functor, typename... argument_types>
        auto then(wait_priority priority, Functor&& func, argument_types... args) const
        {
            wait(priority);
            return std::invoke(std::forward<Functor>(func), std::forward<argument_types>(args)...);
        }

        virtual ~async_operation() = default;
    };
}
