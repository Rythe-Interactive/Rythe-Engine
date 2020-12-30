#pragma once
#include <core/types/primitives.hpp>
#include <atomic>
#include <thread>
#include <unordered_map>
#include <core/async/transferable_atomic.hpp>

#include <Optick/optick.h>

/**
 * @file ring_sync_lock.hpp
 */

namespace legion::core::async
{
	/**@class ring_sync_lock
	 * @brief Allows other threads to synchronize to the owning thread.
	 */
	struct ring_sync_lock
	{
	private:
		const uint m_maxRank;
		std::atomic_uint m_rank;
		const std::thread::id owningThread;
		rw_spinlock m_waitersLock;
		std::unordered_map<uint, transferable_atomic<uint>> m_waiters;
        std::atomic_uint m_subscribers;
        std::atomic_bool m_release;

	public:
        ring_sync_lock(uint maxRank = UINT32_MAX) : m_maxRank(maxRank), owningThread(std::this_thread::get_id()) { m_release.store(false, std::memory_order_relaxed); }
		ring_sync_lock(const ring_sync_lock&) = delete;
		ring_sync_lock(ring_sync_lock&) = delete;
		ring_sync_lock& operator=(const ring_sync_lock&) = delete;
		ring_sync_lock& operator=(ring_sync_lock&&) = delete;

		void sync()
		{
            OPTICK_EVENT();
            uint rank = m_rank.load(std::memory_order_acquire); // Fetch the current rank at which the lock should be.
			uint nextRank = (rank + 1) % m_maxRank; // Calculate the next rank at which the lock should be.

			if (std::this_thread::get_id() == owningThread) // Check if we're here to wait or to free.
			{
                {
                    readonly_guard rguard(m_waitersLock);
                    if (!m_waiters.count(rank)) // If no one was waiting then we can just continue.
                        return;
                }

                bool wait = true;

                while (wait)
                {
                    readonly_guard rguard(m_waitersLock);
                    wait = m_waiters[rank]->load(std::memory_order_acquire) < m_subscribers.load(std::memory_order_relaxed);
                    std::this_thread::yield();
                }

                m_rank.store(nextRank, std::memory_order::memory_order_release); // Update the rank of the lock, also freeing all waiting threads.

                wait = true;

                while (wait) // Wait until all waiting threads have continued.
                {
                    readonly_guard rguard(m_waitersLock);
                    wait = m_waiters[rank]->load(std::memory_order_acquire) != 0;
                    std::this_thread::yield();
                }

				readwrite_guard wguard(m_waitersLock);
				m_waiters.erase(rank); // Remove previous rank in order to save memory.
			}
			else // We are here to wait for sync.
			{
				{
                    readonly_guard rguard(m_waitersLock); // If we are the first waiting then we need to insert the wait list for the new rank. This needs write permission.
                    if (!m_waiters.count(rank))
                    {
                        readwrite_guard wguard(m_waitersLock);
                        m_waiters[rank]->store(0, std::memory_order_release);
                    }
				}

				{
					readonly_guard rguard(m_waitersLock);
					m_waiters[rank]->fetch_add(1, std::memory_order_acq_rel); // Mark that we are waiting.
				}

                uint previousRank = rank;

				while (rank != nextRank && ((rank + 1) % m_maxRank) == nextRank && !m_release.load(std::memory_order_acquire)) // As long as the rank hasn't moved up we keep waiting.
				{
					rank = m_rank.load(std::memory_order_acquire); // Reload the rank.
                    std::this_thread::yield();
				}

				{
					readonly_guard rguard(m_waitersLock);
					m_waiters[previousRank]->fetch_sub(1, std::memory_order_acq_rel); // Remove ourselves from the waiting list.
				}
			}
		}

        void force_release()
        {
            m_release.store(true, std::memory_order_release);
        }

        void subscribe()
        {
            m_subscribers.fetch_add(1, std::memory_order_acq_rel);
        }

        void unsubscribe()
        {
            m_subscribers.fetch_sub(1, std::memory_order_acq_rel);
        }

		uint waiterCount()
		{
			readonly_guard guard(m_waitersLock);
			return m_waiters[m_rank.load(std::memory_order_acquire)]->load(std::memory_order_acquire);
		}

		std::thread::id ownerThread() { return owningThread; }
	};
}
