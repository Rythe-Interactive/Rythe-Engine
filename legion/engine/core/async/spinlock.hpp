#pragma once
#include <core/platform/platform.hpp>
#include <core/detail/internals.hpp>
#include <core/logging/logging.hpp>
#include <atomic>
#include <mutex> // Anyone who includes this file can also use std::lock_guard

namespace legion::core::async
{
    struct spinlock final
    {
    private:
        static bool m_forceRelease;
        static std::atomic_uint m_lastId;
        static thread_local std::unordered_map<id_type, uint> m_localState;
        std::atomic_bool m_lock = { false };
        uint m_id = m_lastId.fetch_add(1, std::memory_order_relaxed);

    public:
        static void force_release(bool release = true);

        spinlock() = default;

        spinlock(spinlock&& source);

        spinlock& operator=(spinlock&& source);

        void lock() noexcept;

        L_NODISCARD bool try_lock() noexcept;

        void unlock() noexcept;
    };
}
