#pragma once
#include <core/types/primitives.hpp>
#include <atomic>
#include <thread>
#include <unordered_map>
#include <core/async/transferable_atomic.hpp>

namespace args::core::async
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
		readonly_rw_spinlock m_waitersLock;
		std::unordered_map<uint, transferable_atomic<uint>> m_waiters;

	public:
		ring_sync_lock(uint maxRank = UINT32_MAX) : m_maxRank(maxRank), owningThread(std::this_thread::get_id()) {}
		ring_sync_lock(const ring_sync_lock&) = delete;
		ring_sync_lock(ring_sync_lock&) = delete;
		ring_sync_lock& operator=(const ring_sync_lock&) = delete;
		ring_sync_lock& operator=(ring_sync_lock&&) = delete;

		void sync()
		{
			uint rank = m_rank.load(std::memory_order_acquire);
			uint nextRank = (rank + 1) % m_maxRank;

			if (std::this_thread::get_id() == owningThread)
			{
				m_rank.store(nextRank, std::memory_order::memory_order_release);

				readonly_guard rguard(m_waitersLock);
				if (!m_waiters.count(rank))
					return;

				while (m_waiters[rank]->load(std::memory_order_acquire) != 0)
					;

				readwrite_guard wguard(m_waitersLock);
				m_waiters.erase(rank);
			}
			else
			{
				{
					readwrite_guard wguard(m_waitersLock);
					if (!m_waiters.count(rank))
						m_waiters[rank]->store(0, std::memory_order_release);
				}

				{
					readonly_guard rguard(m_waitersLock);
					m_waiters[rank]->fetch_add(1, std::memory_order_acq_rel);
				}

				while (rank != nextRank && ((rank + 1) % m_maxRank) == nextRank)
				{
					rank = m_rank.load(std::memory_order_acquire);
				}

				{
					readonly_guard rguard(m_waitersLock);
					m_waiters[rank]->fetch_sub(1, std::memory_order_acq_rel);
				}
			}
		}

		uint waiterCount()
		{
			readonly_guard guard(m_waitersLock);
			return m_waiters[m_rank.load(std::memory_order_acquire)]->load(std::memory_order_acquire);
		}

		std::thread::id ownerThread() { return owningThread; }
	};
}