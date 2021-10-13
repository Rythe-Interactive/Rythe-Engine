#pragma once
#include <atomic>
#include <mutex>
#include <memory>
#include <optional>
#include <thread>
#include <utility>

#include <core/platform/platform.hpp>
#include <core/types/types.hpp>
#include <core/async/wait_priority.hpp>
#include <core/async/spinlock.hpp>
#include <core/containers/delegate.hpp>
#include <core/common/result.hpp>
#include <core/common/exception.hpp>

namespace legion::core::async
{
    struct async_progress_base
    {
    protected:
        const size_type m_size;
        std::atomic<size_type> m_progress;

        template<typename T>
        constexpr static T precision_scale = static_cast<T>(1000);

        void complete_impl() noexcept;

    public:
        constexpr async_progress_base() noexcept : m_size(100u * precision_scale<size_type>), m_progress(0) {}
        constexpr explicit async_progress_base(float size) noexcept : m_size(static_cast<size_type>(size * precision_scale<float>)), m_progress(0) {}

        L_NODISCARD float size() const noexcept;
        L_NODISCARD size_type raw_size() const noexcept;
        L_NODISCARD size_type raw_progress() const noexcept;

        void advance_progress(float progress = 1.f) noexcept;
        void reset(float progress = 0.f) noexcept;
        L_NODISCARD bool is_done() const noexcept;
        L_NODISCARD float progress() const noexcept;
    };

    template<typename ReturnType>
    struct async_progress : public async_progress_base
    {
        using payload_type = ReturnType;
    protected:
        spinlock m_payloadLock;
        std::optional<payload_type> m_payload;

    public:
        constexpr async_progress() noexcept : async_progress_base() {}
        constexpr explicit async_progress(float size) noexcept : async_progress_base(size) {}

        void complete(ReturnType&& value) noexcept
        {
            std::lock_guard guard(m_payloadLock);
            m_payload = std::move(value);
            complete_impl();
        }

        template<typename... Args>
        void complete(Args&&... args) noexcept
        {
            std::lock_guard guard(m_payloadLock);
            m_payload.emplace(std::forward<Args>(args)...);
            complete_impl();
        }

        L_NODISCARD common::result<std::reference_wrapper<payload_type>> get_result()
        {
            std::lock_guard guard(m_payloadLock);
            if (m_payload)
                return std::ref(m_payload.value());
            return legion_exception_msg("Payload of async operation was not ready yet.");
        }
    };

    template<>
    struct async_progress<void> : public async_progress_base
    {
    public:
        constexpr async_progress() noexcept : async_progress_base() {}
        constexpr explicit async_progress(float size) noexcept : async_progress_base(size) {}

        void complete() noexcept { complete_impl(); }
    };

    struct async_operation_base
    {
    protected:
        std::shared_ptr<async_progress_base> m_progress;

    public:
        explicit async_operation_base(const std::shared_ptr<async_progress_base>& progress) noexcept;
        NO_DTOR_RULE5_NOEXCEPT(async_operation_base);
        virtual ~async_operation_base() = default;

        L_NODISCARD bool is_done() const noexcept;

        L_NODISCARD float progress() const noexcept;

        virtual void wait(wait_priority priority = wait_priority_normal) const noexcept;
    };

    template<typename functor, typename payload>
    struct repeating_async_operation : public async_operation_base
    {
    protected:
        functor m_repeater;

    public:
        repeating_async_operation(const std::shared_ptr<async_progress<payload>>& progress, functor&& repeater) : async_operation_base(progress), m_repeater(repeater) {}
        repeating_async_operation() noexcept(std::is_nothrow_default_constructible_v<functor>) = default;
        repeating_async_operation(const repeating_async_operation&) noexcept(std::is_nothrow_copy_constructible_v<functor>) = default;
        repeating_async_operation(repeating_async_operation&&) noexcept(std::is_nothrow_move_constructible_v<functor>) = default;

        repeating_async_operation& operator=(const repeating_async_operation&) noexcept(std::is_nothrow_copy_assignable_v<functor>) = default;
        repeating_async_operation& operator=(repeating_async_operation&&) noexcept(std::is_nothrow_move_assignable_v<functor>) = default;

        virtual ~repeating_async_operation() = default;

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

        L_NODISCARD payload& get_result(wait_priority priority = wait_priority_normal)
        {
            wait(priority);
            return static_cast<async_progress<payload>*>(m_progress.get())->get_result().value();
        }
    };

#if !defined(DOXY_EXCLUDE)
    template<typename functor, typename payload>
    repeating_async_operation(const std::shared_ptr<async_progress<payload>>&, functor&&)->repeating_async_operation<functor, payload>;
#endif

    template<typename functor>
    struct repeating_async_operation<functor, void> : public async_operation_base
    {
    protected:
        functor m_repeater;

    public:
        repeating_async_operation(const std::shared_ptr<async_progress<void>>& progress, functor&& repeater) : async_operation_base(progress), m_repeater(repeater) {}
        repeating_async_operation() noexcept(std::is_nothrow_default_constructible_v<functor>) = default;
        repeating_async_operation(const repeating_async_operation&) noexcept(std::is_nothrow_copy_constructible_v<functor>) = default;
        repeating_async_operation(repeating_async_operation&&) noexcept(std::is_nothrow_move_constructible_v<functor>) = default;

        repeating_async_operation& operator=(const repeating_async_operation&) noexcept(std::is_nothrow_copy_assignable_v<functor>) = default;
        repeating_async_operation& operator=(repeating_async_operation&&) noexcept(std::is_nothrow_move_assignable_v<functor>) = default;

        virtual ~repeating_async_operation() = default;

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
    };

    template<typename payload>
    struct async_operation : public async_operation_base
    {
    public:
        async_operation(const std::shared_ptr<async_progress<payload>>& progress) : async_operation_base(progress) {}

        NO_DTOR_RULE5_NOEXCEPT(async_operation);
        virtual ~async_operation() = default;

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

        L_NODISCARD payload& get_result(wait_priority priority = wait_priority_normal)
        {
            wait(priority);
            return static_cast<async_progress<payload>*>(m_progress.get())->get_result().value();
        }
    };

#if !defined(DOXY_EXCLUDE)
    template<typename payload>
    async_operation(const std::shared_ptr<async_progress<payload>>&)->async_operation<payload>;
#endif

    template<>
    struct async_operation<void> : public async_operation_base
    {
    public:
        async_operation(const std::shared_ptr<async_progress<void>>& progress) : async_operation_base(progress) {}
        NO_DTOR_RULE5_NOEXCEPT(async_operation);

        virtual ~async_operation() = default;

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
    };
}
