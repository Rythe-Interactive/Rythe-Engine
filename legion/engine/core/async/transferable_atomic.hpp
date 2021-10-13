#pragma once
#include <atomic>
#include <core/async/rw_spinlock.hpp>

/**
 * @file transferable_atomic.hpp
 */

namespace legion::core::async
{
    /**@class transferable_atomic
     * @brief Copyable wrapper for std::atomic.
     */
    template <typename T>
    struct transferable_atomic
    {
    private:
        mutable std::atomic<T> m_atomic;
        mutable rw_spinlock m_lock;
    public:
        transferable_atomic() = default;

        explicit transferable_atomic(T val) : m_atomic(val), m_lock() {}

        transferable_atomic(const std::atomic<T>& other) : m_atomic(other.load(std::memory_order_acquire)), m_lock() {}

        transferable_atomic(const transferable_atomic<T>& other) : m_atomic(other->load(std::memory_order_acquire)), m_lock() {}

        transferable_atomic(std::atomic<T>&& other) : m_atomic(other.load(std::memory_order_acquire)), m_lock()
        {
            other.store(T(), std::memory_order_release);
        }

        transferable_atomic(transferable_atomic<T>&& other) : m_atomic(other->load(std::memory_order_acquire)), m_lock()
        {
            other->store(T(), std::memory_order_release);
        }

        ~transferable_atomic() = default;

        rw_spinlock& get_lock() noexcept
        {
            return m_lock;
        }

        transferable_atomic<T>& operator=(const transferable_atomic<T>& other) noexcept
        {
            m_atomic.store(other->load(std::memory_order_acquire), std::memory_order_release);
            return *this;
        }

        transferable_atomic<T>& operator=(const std::atomic<T>& other) noexcept
        {
            m_atomic.store(other.load(std::memory_order_acquire), std::memory_order_release);
            return *this;
        }

        transferable_atomic<T>& operator=(transferable_atomic<T>&& other) noexcept
        {
            m_atomic.store(other->load(std::memory_order_acquire), std::memory_order_release);
            other->store(T(), std::memory_order_release);
            return *this;
        }

        transferable_atomic<T>& operator=(std::atomic<T>&& other) noexcept
        {
            m_atomic.store(other.load(std::memory_order_acquire), std::memory_order_release);
            other.store(T(), std::memory_order_release);
            return *this;
        }

        transferable_atomic<T>& copy(const transferable_atomic<T>& other, std::memory_order loadOrder = std::memory_order_acquire, std::memory_order storeOrder = std::memory_order_release) noexcept
        {
            m_atomic.store(other->load(loadOrder), storeOrder);
            return *this;
        }

        transferable_atomic<T>& move(transferable_atomic<T>&& other, std::memory_order loadOrder = std::memory_order_acquire, std::memory_order storeOrder = std::memory_order_release) noexcept
        {
            m_atomic.store(other->load(loadOrder), storeOrder);
            other->store(T(), storeOrder);
            return *this;
        }

        std::atomic<T>& get() noexcept
        {
            return m_atomic;
        }

        const std::atomic<T>& get() const noexcept
        {
            return m_atomic;
        }

        std::atomic<T>* operator->() noexcept
        {
            return &m_atomic;
        }

        const std::atomic<T>* operator->() const noexcept
        {
            return &m_atomic;
        }
    };
}
