#pragma once
#include <atomic>
#include <unordered_map>
#include <core/types/primitives.hpp>
#include <core/platform/platform.hpp>

/**
 * @file readonly_rw_spinlock.hpp
 */

namespace args::core::async
{
	/**@class readonly_rw_spinlock
	 * @brief Lock used with ::async::readonly_guard and ::async::readwrite_guard.
	 * @note Read-only operations can happen simultaneously without waiting for each other.
	 *		 Read-only operations will only wait for Read-Write operations to be finished.
	 * @note Read-Write operations cannot happen simultaneously and will wait for each other.
	 *		 Read-Write operations will also wait for any Read-only operations to be finished.
	 * @ref args::core::async::readonly_guard
	 * @ref args::core::async::readwrite_guard
	 */
	struct ARGS_API readonly_rw_spinlock
	{
		friend class readonly_guard;
		friend class readwrite_guard;
	private:
		enum read_state { idle = 0, read = 1, write = 2 };
		static std::atomic_uint lastId;
		const uint id;
		std::atomic_int readState;
		std::atomic_int readers;

		static std::unordered_map<uint, int>& localStates();
	public:
		readonly_rw_spinlock();
		readonly_rw_spinlock(const readonly_rw_spinlock&) = delete;
		readonly_rw_spinlock& operator=(readonly_rw_spinlock&&) = delete;
	};

	/**@class readonly_guard
	 * @brief RAII guard that uses ::async::readonly_rw_spinlock to lock for read-only.
	 *        Read-only operations can happen simultaneously without waiting for each other.
	 *		  Read-only operations will only wait for Read-Write operations to be finished.
	 * @ref args::core::async::readonly_rw_spinlock
	 */
	class readonly_guard final
	{
	private:
		readonly_rw_spinlock& lock;
		bool preacquired;

	public:
		/**@brief Creates readonly guard and locks for Read-only.
		 */
		readonly_guard(readonly_rw_spinlock& lock) : lock(lock)
		{
			if (lock.localStates()[lock.id] != readonly_rw_spinlock::idle) // If we're either already reading or writing then the lock doesn't need to be reacquired.
			{
				preacquired = true; // Remember that there's another active guard.
				return;
			}

			preacquired = false;

			// Expect idle as default.
			int state = readonly_rw_spinlock::idle;
			// Report another reader to the lock.
			lock.readers.fetch_add(1, std::memory_order_relaxed);

			// Try to set the lock state to read
			while (!lock.readState.compare_exchange_weak(state, readonly_rw_spinlock::read, std::memory_order_acquire, std::memory_order_relaxed))
			{
				// If the lock state was already on read we can continue without issues, read-only operations are allowed to happen simultaneously.
				if (state == readonly_rw_spinlock::read)
					break;
				else // If the lock was in any other state than idle or read then we need to stay in the CAS loop to prevent writes from happening during our read.
					state = readonly_rw_spinlock::idle;
			}

			lock.localStates()[lock.id] = readonly_rw_spinlock::read; // Set thread_local state to read.

		}

		readonly_guard(const readonly_guard&) = delete;

		/**@brief RAII style unlocks lock from Read-only.
		 */
		~readonly_guard()
		{
			if (preacquired) // Another guard is still alive that will unlock the lock for this thread.
				return;

			// Mark our read as finished.
			lock.readers.fetch_sub(1, std::memory_order_relaxed);

			// If there are no more readers left then the lock state needs to be returned to idle.
			// This allows other (non readonly)locks to acquire access.
			if (lock.readers.load(std::memory_order_acquire) == 0)
				lock.readState.store(readonly_rw_spinlock::idle, std::memory_order_release);

			lock.localStates()[lock.id] = readonly_rw_spinlock::idle; // Set thread_local state to idle.
		}

		readonly_guard& operator=(readonly_guard&&) = delete;
	};

	/**@class readwrite_guard
	 * @brief RAII guard that uses ::async::readonly_rw_spinlock to lock for read/write.
	 *        Read-Write operations cannot happen simultaneously and will wait for each other.
	 *		  Read-Write operations will also wait for any Read-only operations to be finished.
	 * @ref args::core::async::readonly_rw_spinlock
	 */
	class readwrite_guard final
	{
	private:
		readonly_rw_spinlock& lock;
		bool preacquired;

	public:
		/**@brief Creates readonly guard and locks for Read-Write.
		 */
		readwrite_guard(readonly_rw_spinlock& lock) : lock(lock)
		{
			if (lock.localStates()[lock.id] == readonly_rw_spinlock::read) // If we're currently only acquired for read we need to stop reading before requesting rw.
			{
				// Mark our read as finished.
				lock.readers.fetch_sub(1, std::memory_order_relaxed);

				// If there are no more readers left then the lock state needs to be returned to idle.
				// This allows other (non readonly)locks to acquire access.
				if (lock.readers.load(std::memory_order_acquire) == 0)
					lock.readState.store(readonly_rw_spinlock::idle, std::memory_order_release);
			}
			else if (lock.localStates()[lock.id] == readonly_rw_spinlock::write) // If we're already writing then we don't need to reacquire the lock.
			{
				preacquired = true; // Remember that there's another active guard.
				return;
			}

			preacquired = false;

			// Expect idle as default.
			int state = readonly_rw_spinlock::idle;

			// Try to set the lock state to write.
			while (!lock.readState.compare_exchange_weak(state, readonly_rw_spinlock::write, std::memory_order_acquire, std::memory_order_relaxed))
				// If lock state is any other state than idle then we cannot acquire access to write, thus we need to stay in the CAS loop.
				state = readonly_rw_spinlock::idle;


			lock.localStates()[lock.id] = readonly_rw_spinlock::write; // Set thread_local state to write.
		}

		readwrite_guard(const readwrite_guard&) = delete;

		/**@brief RAII style unlocks lock from Read-Write.
		 */
		~readwrite_guard()
		{
			if (preacquired) // Another guard is still alive that will unlock the lock for this thread.
				return;

			// We should be the only one to have had access so we can safely write to the lock state and return it to idle.
			lock.readState.store(readonly_rw_spinlock::idle, std::memory_order_release);

			lock.localStates()[lock.id] = readonly_rw_spinlock::idle; // Set thread_local state to idle.
		}

		readwrite_guard& operator=(readwrite_guard&&) = delete;
	};
}