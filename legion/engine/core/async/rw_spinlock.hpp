#pragma once
#include <atomic>
#include <tuple>
#include <unordered_map>
#include <array>
#include <thread>
#include <functional>
#include <core/types/primitives.hpp>
#include <core/platform/platform.hpp>
#include <core/containers/sparse_set.hpp>
#include <core/detail/internals.hpp>

/**
 * @file rw_spinlock.hpp
 */

namespace legion::core::async
{
    enum lock_state { idle = 0, read = 1, write = -1 };

    /**@class rw_spinlock
     * @brief Lock used with ::async::readonly_guard and ::async::readwrite_guard.
     * @note Read-only operations can happen simultaneously without waiting for each other.
     *		 Read-only operations will only wait for Read-Write operations to be finished.
     * @note Read-Write operations cannot happen simultaneously and will wait for each other.
     *		 Read-Write operations will also wait for any Read-only operations to be finished.
     * @ref legion::core::async::readonly_guard
     * @ref legion::core::async::readwrite_guard
     * @ref legion::core::async::readonly_multiguard
     * @ref legion::core::async::readwrite_multiguard
     * @ref legion::core::async::mixed_multiguard
     */
    struct rw_spinlock final
    {
    private:
        static bool m_forceRelease;
        static std::atomic_uint m_lastId;

        static thread_local std::unique_ptr<std::unordered_map<uint, int>> m_localWriters;
        static thread_local std::unique_ptr<std::unordered_map<uint, int>> m_localReaders;
        static thread_local std::unique_ptr<std::unordered_map<uint, lock_state>> m_localState;

        uint m_id = m_lastId.fetch_add(1, std::memory_order_relaxed);
        // State of the lock. -1 means that a thread has write permission. 0 means that the lock is unlocked. 1+ means that there are N amount of readers.
        std::atomic_int m_lockState = { 0 };

        void read_lock()
        {
            if (m_forceRelease)
                return;

            if ((*m_localState)[m_id] != lock_state::idle) // If we're either already reading or writing then the lock doesn't need to be reacquired.
            {
                // Report another local reader to the lock.
                (*m_localReaders)[m_id]++;
                return;
            }

            int state;

            while (true)
            {
                // Read the current value and continue waiting until we're in a lockable state.
                while ((state = m_lockState.load(std::memory_order_relaxed)) == lock_state::write)
                    L_PAUSE_INSTRUCTION();

                // Try to add a reader to the lock state. If the lock succeeded then we can continue.
                if (m_lockState.compare_exchange_weak(state, state + lock_state::read, std::memory_order_acquire, std::memory_order_relaxed))
                    break;
            }

            // Report another reader to the lock.
            (*m_localReaders)[m_id]++;
            (*m_localState)[m_id] = lock_state::read; // Set thread_local state to read.
        }

        bool read_try_lock()
        {
            if (m_forceRelease)
                return true;

            if ((*m_localState)[m_id] != lock_state::idle) // If we're either already reading or writing then the lock doesn't need to be reacquired.
            {
                // Report another local reader to the lock.
                (*m_localReaders)[m_id]++;
                return true;
            }

            // Expect idle as default.
            int state;

            if ((state = m_lockState.load(std::memory_order_relaxed)) == lock_state::write || // Check if we can lock at all first to reduce LSU abuse on SMT CPUs if this occurs in a try_lock loop.
                !m_lockState.compare_exchange_strong(state, state + lock_state::read, std::memory_order_acquire, std::memory_order_relaxed)) // Try to add a reader to the lock state.
                return false;

            // Report another reader to the lock.
            (*m_localReaders)[m_id]++;
            (*m_localState)[m_id] = lock_state::read; // Set thread_local state to read.
            return true;
        }

        void write_lock()
        {
            if (m_forceRelease)
                return;

            if ((*m_localState)[m_id] == lock_state::read) // If we're currently only acquired for read we need to stop reading before requesting rw.
            {
                read_unlock();
                (*m_localReaders)[m_id]++;
            }
            else if ((*m_localState)[m_id] == lock_state::write) // If we're already writing then we don't need to reacquire the lock.
            {
                (*m_localWriters)[m_id]++;
                return;
            }

            int state;

            while (true)
            {
                // Read the current value and continue waiting until we're in a lockable state.
                while ((state = m_lockState.load(std::memory_order_relaxed)) != lock_state::idle)
                    L_PAUSE_INSTRUCTION();

                // Try to set the lock state to write. If the lock succeeded then we can continue.
                if (m_lockState.compare_exchange_weak(state, lock_state::write, std::memory_order_acquire, std::memory_order_relaxed))
                    break;
            }

            (*m_localWriters)[m_id]++;
            (*m_localState)[m_id] = lock_state::write; // Set thread_local state to write.
        }

        bool write_try_lock()
        {
            if (m_forceRelease)
                return true;

            bool relock = false;
            if ((*m_localState)[m_id] == lock_state::read) // If we're currently only acquired for read we need to stop reading before requesting rw.
            {
                relock = true;
                read_unlock();
                (*m_localReaders)[m_id]++;
            }
            else if ((*m_localState)[m_id] == lock_state::write) // If we're already writing then we don't need to reacquire the lock.
            {
                (*m_localWriters)[m_id]++;
                return true;
            }

            // Expect idle as default.
            int state = lock_state::idle;

            if ((state = m_lockState.load(std::memory_order_relaxed)) != lock_state::idle || // Check if we can lock at all first to reduce LSU abuse on SMT CPUs if this occurs in a try_lock loop.
                !m_lockState.compare_exchange_strong(state, lock_state::write, std::memory_order_acquire, std::memory_order_relaxed)) // Try to set the lock state to write.
            {
                if (relock)
                {
                    (*m_localReaders)[m_id]--;
                    read_lock();
                }
                return false;
            }

            (*m_localWriters)[m_id]++;
            (*m_localState)[m_id] = lock_state::write; // Set thread_local state to write.
            return true;
        }

        void read_unlock()
        {
            if (m_forceRelease)
                return;

            (*m_localReaders)[m_id]--;

            if ((*m_localReaders)[m_id] > 0 || (*m_localWriters)[m_id] > 0) // Another local guard is still alive that will unlock the lock for this thread.
            {
                return;
            }

            m_lockState.fetch_sub(lock_state::read, std::memory_order_release);

            (*m_localState)[m_id] = lock_state::idle; // Set thread_local state to idle.
        }

        void write_unlock()
        {
            if (m_forceRelease)
                return;

            (*m_localWriters)[m_id]--;

            if ((*m_localWriters)[m_id] > 0) // Another local guard is still alive that will unlock the lock for this thread.
            {
                return;
            }
            else if ((*m_localReaders)[m_id] > 0)
            {
                m_lockState.store(lock_state::read, std::memory_order_release);
                (*m_localState)[m_id] = lock_state::read; // Set thread_local state to idle.
                return;
            }

            m_lockState.store(lock_state::idle, std::memory_order_release);
            (*m_localState)[m_id] = lock_state::idle; // Set thread_local state to idle.
        }

    public:
        static void force_release()
        {
            m_forceRelease = true;
        }

        rw_spinlock() = default;

        rw_spinlock(rw_spinlock&& source)
        {
            if (m_forceRelease)
                return;

            if (!m_localWriters.get())
                m_localWriters = std::make_unique<std::unordered_map<uint, int>>();
            if (!m_localReaders.get())
                m_localReaders = std::make_unique<std::unordered_map<uint, int>>();
            if (!m_localState.get())
                m_localState = std::make_unique<std::unordered_map<uint, lock_state>>();

            assert_msg("Attempted to move a rw_spinlockthat was locked.", source.m_lockState.load(std::memory_order_relaxed) == lock_state::idle);
            m_id = source.m_id;
        }

        rw_spinlock& operator=(rw_spinlock&& source)
        {
            if (m_forceRelease)
                return *this;

            if (!m_localWriters.get())
                m_localWriters = std::make_unique<std::unordered_map<uint, int>>();
            if (!m_localReaders.get())
                m_localReaders = std::make_unique<std::unordered_map<uint, int>>();
            if (!m_localState.get())
                m_localState = std::make_unique<std::unordered_map<uint, lock_state>>();

            assert_msg("Attempted to move a rw_spinlockthat was locked.", source.m_lockState.load(std::memory_order_relaxed) == lock_state::idle);
            m_id = source.m_id;
            return *this;
        }

        rw_spinlock(const rw_spinlock&) = delete;
        rw_spinlock& operator=(const rw_spinlock&) = delete;

        /**@brief Lock for a certain permission level. (locking for idle does nothing)
         * @note Locking stacks, locking for readonly multiple times will remain readonly.
         *		 Locking for write after already being locked for readonly in the same thread will attempt to elevate lock permission of this thread to write.
         *		 Locking for write multiple times will remain in write.
         * @param permissionLevel
         */
        void lock(lock_state permissionLevel = lock_state::write)
        {
            if (m_forceRelease)
                return;

            if (!m_localWriters.get())
                m_localWriters = std::make_unique<std::unordered_map<uint, int>>();
            if (!m_localReaders.get())
                m_localReaders = std::make_unique<std::unordered_map<uint, int>>();
            if (!m_localState.get())
                m_localState = std::make_unique<std::unordered_map<uint, lock_state>>();

            switch (permissionLevel)
            {
            case lock_state::read:
                return read_lock();
            case lock_state::write:
                return write_lock();
            default:
                return;
            }
        }

        /**@brief Try to lock for a certain permission level. If it fails it will return false otherwise true. (locking for idle does nothing)
         * @note Locking stacks, locking for readonly multiple times will remain readonly.
         *		 Locking for write after already being locked for readonly in the same thread will attempt to elevate lock permission of this thread to write.
         *		 Locking for write multiple times will remain in write.
         * @param permissionLevel
         * @return bool True when locked.
         */
        bool try_lock(lock_state permissionLevel = lock_state::write)
        {
            if (m_forceRelease)
                return true;

            if (!m_localWriters.get())
                m_localWriters = std::make_unique<std::unordered_map<uint, int>>();
            if (!m_localReaders.get())
                m_localReaders = std::make_unique<std::unordered_map<uint, int>>();
            if (!m_localState.get())
                m_localState = std::make_unique<std::unordered_map<uint, lock_state>>();

            switch (permissionLevel)
            {
            case lock_state::read:
                return read_try_lock();
            case lock_state::write:
                return write_try_lock();
            default:
                return false;
            }
        }

        /**@brief Unlock from a certain permission level.
         * @note If both read and write locks have been requested before and write is unlocked then the lock will return to readonly state.
         * @param permissionLevel
         */
        void unlock(lock_state permissionLevel = lock_state::write)
        {
            if (m_forceRelease)
                return;

            if (!m_localWriters.get())
                m_localWriters = std::make_unique<std::unordered_map<uint, int>>();
            if (!m_localReaders.get())
                m_localReaders = std::make_unique<std::unordered_map<uint, int>>();
            if (!m_localState.get())
                m_localState = std::make_unique<std::unordered_map<uint, lock_state>>();

            switch (permissionLevel)
            {
            case legion::core::async::lock_state::read:
                return read_unlock();
            case legion::core::async::lock_state::write:
                return write_unlock();
            default:
                return;
            }
        }

        /** @brief Locks the rw_spinlockfor shared ownership, blocks if the rw_spinlockis not available
         */
        void lock_shared()
        {
            if (m_forceRelease)
                return;

            if (!m_localWriters.get())
                m_localWriters = std::make_unique<std::unordered_map<uint, int>>();
            if (!m_localReaders.get())
                m_localReaders = std::make_unique<std::unordered_map<uint, int>>();
            if (!m_localState.get())
                m_localState = std::make_unique<std::unordered_map<uint, lock_state>>();

            return read_lock();
        }

        /** @brief Tries to lock the rw_spinlockfor shared ownership, returns if the rw_spinlockis not available
         */
        bool try_lock_shared()
        {
            if (m_forceRelease)
                return true;

            if (!m_localWriters.get())
                m_localWriters = std::make_unique<std::unordered_map<uint, int>>();
            if (!m_localReaders.get())
                m_localReaders = std::make_unique<std::unordered_map<uint, int>>();
            if (!m_localState.get())
                m_localState = std::make_unique<std::unordered_map<uint, lock_state>>();

            return read_try_lock();
        }

        /** @brief Unlocks the mutex (shared ownership)
         */
        void unlock_shared()
        {
            if (m_forceRelease)
                return;

            if (!m_localWriters.get())
                m_localWriters = std::make_unique<std::unordered_map<uint, int>>();
            if (!m_localReaders.get())
                m_localReaders = std::make_unique<std::unordered_map<uint, int>>();
            if (!m_localState.get())
                m_localState = std::make_unique<std::unordered_map<uint, lock_state>>();

            return read_unlock();
        }

        /**@brief Execute a function inside a critical section locked by a certain guard.
         * @tparam Guard Guard type to lock the lock with.
         * @param func Function to execute.
         * @return Return value of func.
         */
        template<typename Guard, typename Func>
        auto critical_section(const Func& func) -> decltype(auto)
        {
            Guard guard(*this);
            return std::invoke(func);
        }
    };

    /**@class readonly_guard
     * @brief RAII guard that uses ::async::rw_spinlock to lock for read-only.
     * @note Read-only operations can happen simultaneously without waiting for each other.
     *		 Read-only operations will only wait for Read-Write operations to be finished.
     * @ref legion::core::async::rw_spinlock
     */
    class readonly_guard final
    {
    private:
        rw_spinlock& m_lock;

    public:
        /**@brief Creates readonly guard and locks for Read-only.
         */
        readonly_guard(rw_spinlock& lock) : m_lock(lock)
        {
            m_lock.lock(read);
        }

        readonly_guard(const readonly_guard&) = delete;

        /**@brief RAII style unlocks lock from Read-only.
         */
        ~readonly_guard()
        {
            m_lock.unlock(read);
        }

        readonly_guard& operator=(readonly_guard&&) = delete;
    };

    /**@class readonly_multiguard
     * @brief RAII guard that uses multiple ::async::readonly_rw_spinlocks to lock them all for read-only. (similar to std::lock)
     * @note Read-only operations can happen simultaneously without waiting for each other.
     *		 Read-only operations will only wait for Read-Write operations to be finished.
     * @ref legion::core::async::rw_spinlock
     */
    template<size_type S>
    class readonly_multiguard final
    {
    private:
        std::array<rw_spinlock*, S> m_locks;

    public:
        /**@brief Creates readonly multi-guard and locks for Read-only.
         */
        template<typename lock_type1 = rw_spinlock, typename lock_type2 = rw_spinlock, typename... lock_typesN>
        readonly_multiguard(lock_type1& lock1, lock_type2& lock2, lock_typesN&... locks) : m_locks{ {&lock1, &lock2, &locks...} }
        {
            int lastLocked = -1; // Index to the last locked lock.

            bool locked = true;
            do
            {
                for (int i = 0; i <= lastLocked; i++) // If we failed to lock all locks we need to unlock the ones we did lock.
                    m_locks[i]->unlock(read);

                // Reset variables
                locked = true;
                lastLocked = -1;

                // Try to lock all locks.
                for (int i = 0; i < m_locks.size(); i++)
                {
                    if (m_locks[i]->try_lock(read))
                    {
                        lastLocked = i;
                    }
                    else
                    {
                        locked = false;
                        break;
                    }
                }
            } while (!locked);
        }

        readonly_multiguard(const readonly_multiguard&) = delete;

        /**@brief RAII style unlocks lock from Read-only.
         */
        ~readonly_multiguard()
        {
            for (rw_spinlock* lock : m_locks)
                lock->unlock(read);
        }

        readonly_multiguard& operator=(readonly_multiguard&&) = delete;
    };

#if !defined(DOXY_EXCLUDE)
    template<typename... types>
    readonly_multiguard(types...)->readonly_multiguard<sizeof...(types)>;
#endif
    /**@class readwrite_guard
     * @brief RAII guard that uses ::async::rw_spinlock to lock for read-write.
     * @note Read-Write operations cannot happen simultaneously and will wait for each other.
     *		 Read-Write operations will also wait for any Read-only operations to be finished.
     * @ref legion::core::async::rw_spinlock
     */
    class readwrite_guard final
    {
    private:
        rw_spinlock& m_lock;

    public:
        /**@brief Creates read-write guard and locks for Read-Write.
         */
        readwrite_guard(rw_spinlock& lock) : m_lock(lock)
        {
            m_lock.lock(write);
        }

        readwrite_guard(const readwrite_guard&) = delete;

        /**@brief RAII style unlocks lock from Read-Write.
         */
        ~readwrite_guard()
        {
            m_lock.unlock(write);
        }

        readwrite_guard& operator=(readwrite_guard&&) = delete;
    };


    /**@class readwrite_multiguard
     * @brief RAII guard that uses multiple ::async::readonly_rw_spinlocks to lock them all for read-write. (similar to std::lock)
     * @note Read-Write operations cannot happen simultaneously and will wait for each other.
     *		 Read-Write operations will also wait for any Read-only operations to be finished.
     * @ref legion::core::async::rw_spinlock
     */
    template<size_type S>
    class readwrite_multiguard final
    {
    private:
        std::array<rw_spinlock*, S> m_locks;

    public:
        /**@brief Creates read-write multi-guard and locks for Read-Write.
         */
        template<typename lock_type1, typename lock_type2, typename... lock_typesN>
        readwrite_multiguard(lock_type1& lock1, lock_type2& lock2, lock_typesN&... locks) : m_locks{ {&lock1, &lock2, &locks...} }
        {
            int lastLocked = -1; // Index to the last locked lock.

            bool locked = true;
            do
            {
                for (int i = 0; i <= lastLocked; i++) // If we failed to lock all locks we need to unlock the ones we did lock.
                    m_locks[i]->unlock(write);

                // Reset variables
                locked = true;
                lastLocked = -1;

                // Try to lock all locks.
                for (int i = 0; i < m_locks.size(); i++)
                {
                    if (m_locks[i]->try_lock(write))
                    {
                        lastLocked = i;
                    }
                    else
                    {
                        locked = false;
                        break;
                    }
                }
            } while (!locked);
        }

        readwrite_multiguard(const readwrite_multiguard&) = delete;

        /**@brief RAII style unlocks lock from Read-Write.
         */
        ~readwrite_multiguard()
        {
            for (rw_spinlock* lock : m_locks)
                lock->unlock(write);
        }

        readwrite_multiguard& operator=(readwrite_multiguard&&) = delete;
    };

    template<typename... types>
    readwrite_multiguard(types...)->readwrite_multiguard<sizeof...(types)>;

    /**@class mixed_multiguard
     * @brief RAII guard that uses multiple ::async::readonly_rw_spinlocks to lock them all for user specified permissions. (similar to std::lock)
     * @note Read-only operations can happen simultaneously without waiting for each other.
     *		 Read-only operations will only wait for Read-Write operations to be finished.
     * @note Read-Write operations cannot happen simultaneously and will wait for each other.
     *		 Read-Write operations will also wait for any Read-only operations to be finished.
     * @ref legion::core::async::rw_spinlock
     */
    template<size_type S>
    class mixed_multiguard final
    {
    private:
        std::array<rw_spinlock*, S / 2> m_locks;
        std::array<lock_state, S / 2> m_states;

        // Recursive function for filling the arrays with the neccessary data from the template arguments.
        template<size_type I, typename... types>
        void fill(rw_spinlock& lock, lock_state state, types&&... args)
        {
            if constexpr (I > 2)
            {
                fill<I - 2>(args...);
            }

            m_locks[(I / 2) - 1] = &lock;
            m_states[(I / 2) - 1] = state;
        }

    public:
        /**@brief Creates readonly multi-guard and locks for specified permissions.
         * @note Argument order should be as follows: (rw_spinlock&, lock_state, rw_spinlock&, lock_state, ...)
         */
        template<typename... types>
        explicit mixed_multiguard(types&&... arguments)
        {
            static_assert(sizeof...(types) % 2 == 0, "Argument order should be (lock, lock-state, lock, lock-state). Argument count should thus be even.");

            fill<sizeof...(types)>(arguments...);

            int lastLocked = -1; // Index to the last locked lock.

            bool locked = true;
            do
            {
                for (int i = 0; i <= lastLocked; i++) // If we failed to lock all locks we need to unlock the ones we did lock.
                    m_locks[i]->unlock(m_states[i]);

                // Reset variables
                locked = true;
                lastLocked = -1;

                // Try to lock all locks.
                for (int i = 0; i < m_locks.size(); i++)
                {
                    if (m_locks[i]->try_lock(m_states[i]))
                    {
                        lastLocked = i;
                    }
                    else
                    {
                        locked = false;
                        break;
                    }
                }
            } while (!locked);
        }

        mixed_multiguard(const mixed_multiguard&) = delete;
        mixed_multiguard(mixed_multiguard&&) = delete;

        /**@brief RAII style unlocks lock from specified permissions.
         */
        ~mixed_multiguard()
        {
            for (int i = 0; i < m_locks.size(); i++)
                m_locks[i]->unlock(m_states[i]);
        }

        mixed_multiguard& operator=(mixed_multiguard&&) = delete;
        mixed_multiguard& operator=(const mixed_multiguard&) = delete;
    };

    // CTAD so you don't need to input the size of the guard parameters.
    template<typename... types>
    mixed_multiguard(types...)->mixed_multiguard<sizeof...(types)>;
}
