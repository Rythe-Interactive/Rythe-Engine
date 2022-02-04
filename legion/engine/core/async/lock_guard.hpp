#pragma once
#include <mutex>

#include <core/platform/platform.hpp>

namespace legion::core::async
{
    template <class LockType>
    class L_NODISCARD lock_guard { // class with destructor that unlocks a mutex
    public:
        using mutex_type = LockType;
        using lock_type = LockType;

        explicit lock_guard(lock_type& lck) : m_lock(lck) { // construct and lock
            m_lock.lock();
        }

        lock_guard(lock_type& lck, std::adopt_lock_t) : m_lock(lck) {} // construct but don't lock

        ~lock_guard() noexcept {
            m_lock.unlock();
        }

        lock_guard(const lock_guard&) = delete;
        lock_guard& operator=(const lock_guard&) = delete;

    private:
        lock_type& m_lock;
    };

#if !defined(DOXY_EXCLUDE)
    // CTAD so you don't need to input the type of the lock.
    template<typename LockType>
    lock_guard(LockType&)->lock_guard<LockType>;
#endif
}
