#pragma once
#include <atomic>
#include <tuple>
#include <unordered_map>
#include <array>
#include <core/types/primitives.hpp>
#include <core/platform/platform.hpp>
#include <core/containers/sparse_set.hpp>

/**
 * @file readonly_rw_spinlock.hpp
 */

namespace args::core::async
{
	enum lock_state { idle = 0, read = 1, write = 2 };

	/**@class readonly_rw_spinlock
	 * @brief Lock used with ::async::readonly_guard and ::async::readwrite_guard.
	 * @note Read-only operations can happen simultaneously without waiting for each other.
	 *		 Read-only operations will only wait for Read-Write operations to be finished.
	 * @note Read-Write operations cannot happen simultaneously and will wait for each other.
	 *		 Read-Write operations will also wait for any Read-only operations to be finished.
	 * @ref args::core::async::readonly_guard
	 * @ref args::core::async::readwrite_guard
	 * @ref args::core::async::readonly_multiguard
	 * @ref args::core::async::readwrite_multiguard
	 * @ref args::core::async::mixed_multiguard
	 */
	struct readonly_rw_spinlock
	{
	private:
		inline static std::atomic_uint lastId = 0;
		const uint id;
		std::atomic_int lockState;
		std::atomic_int readers;

		inline static thread_local std::unordered_map<uint, int> localWriters;
		inline static thread_local std::unordered_map<uint, int> localReaders;
		inline static thread_local std::unordered_map<uint, lock_state> localState;

		void read_lock()
		{
			// Report another reader to the lock.
			readers.fetch_add(1, std::memory_order_relaxed);
			localReaders[id]++;

			if (localState[id] != lock_state::idle) // If we're either already reading or writing then the lock doesn't need to be reacquired.
			{
				return;
			}

			// Expect idle as default.
			int state = lock_state::idle;

			// Try to set the lock state to read
			while (!lockState.compare_exchange_weak(state, lock_state::read, std::memory_order_acquire, std::memory_order_relaxed))
			{
				// If the lock state was already on read we can continue without issues, read-only operations are allowed to happen simultaneously.
				if (state == lock_state::read)
					break;
				else // If the lock was in any other state than idle or read then we need to stay in the CAS loop to prevent writes from happening during our read.
					state = lock_state::idle;
			}

			localState[id] = lock_state::read; // Set thread_local state to read.
		}

		bool read_try_lock()
		{
			if (localState[id] != lock_state::idle) // If we're either already reading or writing then the lock doesn't need to be reacquired.
			{
				readers.fetch_add(1, std::memory_order_relaxed);
				localReaders[id]++;
				return true;
			}

			// Expect idle as default.
			int state = lock_state::idle;

			// Try to set the lock state to read
			if (!lockState.compare_exchange_weak(state, lock_state::read, std::memory_order_acquire, std::memory_order_relaxed))
			{
				// If the lock state was already on read we can continue without issues, read-only operations are allowed to happen simultaneously.
				if (state != lock_state::read)
					return false;
			}

			readers.fetch_add(1, std::memory_order_relaxed);
			localReaders[id]++;
			localState[id] = lock_state::read; // Set thread_local state to read.
			return true;
		}

		void write_lock()
		{
			localWriters[id]++;

			if (localState[id] == lock_state::read) // If we're currently only acquired for read we need to stop reading before requesting rw.
			{
				// Mark our read as finished.
				readers.fetch_sub(1, std::memory_order_relaxed);

				// If there are no more readers left then the lock state needs to be returned to idle.
				// This allows other (non readonly)locks to acquire access.
				if (readers.load(std::memory_order_acquire) == 0)
					lockState.store(lock_state::idle, std::memory_order_release);
			}
			else if (localState[id] == lock_state::write) // If we're already writing then we don't need to reacquire the lock.
			{
				return;
			}

			// Expect idle as default.
			int state = lock_state::idle;

			// Try to set the lock state to write.
			while (!lockState.compare_exchange_weak(state, lock_state::write, std::memory_order_acquire, std::memory_order_relaxed))
				// If lock state is any other state than idle then we cannot acquire access to write, thus we need to stay in the CAS loop.
				state = lock_state::idle;


			localState[id] = lock_state::write; // Set thread_local state to write.
		}

		bool write_try_lock()
		{
			if (localState[id] == lock_state::read) // If we're currently only acquired for read we need to stop reading before requesting rw.
			{
				// Mark our read as finished.
				readers.fetch_sub(1, std::memory_order_relaxed);

				// If there are no more readers left then the lock state needs to be returned to idle.
				// This allows other (non readonly)locks to acquire access.
				if (readers.load(std::memory_order_acquire) == 0)
					lockState.store(lock_state::idle, std::memory_order_release);
			}
			else if (localState[id] == lock_state::write) // If we're already writing then we don't need to reacquire the lock.
			{
				localWriters[id]++;
				return true;
			}

			// Expect idle as default.
			int state = lock_state::idle;

			// Try to set the lock state to write.
			if (!lockState.compare_exchange_weak(state, lock_state::write, std::memory_order_acquire, std::memory_order_relaxed))
			{
				if (localReaders[id] > 0)
				{
					readers.fetch_add(1, std::memory_order_relaxed);
					state = lock_state::idle;
					// Try to set the lock state to read
					while (!lockState.compare_exchange_weak(state, lock_state::read, std::memory_order_acquire, std::memory_order_relaxed))
					{
						// If the lock state was already on read we can continue without issues, read-only operations are allowed to happen simultaneously.
						if (state == lock_state::read)
							break;
						else // If the lock was in any other state than idle or read then we need to stay in the CAS loop to prevent writes from happening during our read.
							state = lock_state::idle;
					}

					localState[id] = lock_state::read;
				}
				return false;
			}

			localWriters[id]++;
			localState[id] = lock_state::write; // Set thread_local state to write.
			return true;
		}

		void read_unlock()
		{
			localReaders[id]--;
			// Mark our read as finished.
			readers.fetch_sub(1, std::memory_order_relaxed);

			if (localReaders[id] > 0 || localWriters[id] > 0) // Another local guard is still alive that will unlock the lock for this thread.
			{
				return;
			}

			// If there are no more readers left then the lock state needs to be returned to idle.
			// This allows other (non readonly)locks to acquire access.
			if (readers.load(std::memory_order_acquire) == 0)
				lockState.store(lock_state::idle, std::memory_order_release);

			localState[id] = lock_state::idle; // Set thread_local state to idle.
		}

		void write_unlock()
		{
			localWriters[id]--;
			if (localWriters[id] > 0) // Another write guard is still alive that will unlock the lock for this thread.
			{
				return;
			}
			else if (localReaders[id] > 0) // read permission was granted before write request, we should return to read instead of idle after write is finished.
			{
				// We should be the only one to have had access so we can safely write to the lock state and readers.
				readers.fetch_add(1, std::memory_order_relaxed);
				lockState.store(lock_state::read, std::memory_order_release);

				localState[id] = lock_state::read; // Set thread_local state back to read.
			}
			else
			{
				// We should be the only one to have had access so we can safely write to the lock state and return it to idle.
				lockState.store(lock_state::idle, std::memory_order_release);

				localState[id] = lock_state::idle; // Set thread_local state to idle.
			}
		}

	public:
		readonly_rw_spinlock() : id(lastId.fetch_add(1, std::memory_order_relaxed))
		{
			localState[id] = lock_state::idle;

			lockState.store(lock_state::idle, std::memory_order_relaxed);
			readers.store(0, std::memory_order_relaxed);
		}

		readonly_rw_spinlock(const readonly_rw_spinlock&) = delete;
		readonly_rw_spinlock(readonly_rw_spinlock&) = delete;
		readonly_rw_spinlock& operator=(const readonly_rw_spinlock&) = delete;
		readonly_rw_spinlock& operator=(readonly_rw_spinlock&&) = delete;

		/**@brief Lock for a certain permission level. (locking for idle does nothing)
		 * @note Locking stacks, locking for readonly multiple times will remain readonly.
		 *		 Locking for write after already being locked for readonly in the same thread will attempt to elevate lock permission of this thread to write.
		 *		 Locking for write multiple times will remain in write.
		 * @param permissionLevel
		 */
		void lock(lock_state permissionLevel)
		{
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
		bool try_lock(lock_state permissionLevel)
		{
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
		void unlock(lock_state permissionLevel)
		{
			switch (permissionLevel)
			{
			case args::core::async::lock_state::read:
				return read_unlock();
			case args::core::async::lock_state::write:
				return write_unlock();
			default:
				return;
			}
		}
	};



	/**@class readonly_guard
	 * @brief RAII guard that uses ::async::readonly_rw_spinlock to lock for read-only.
	 * @note Read-only operations can happen simultaneously without waiting for each other.
	 *		 Read-only operations will only wait for Read-Write operations to be finished.
	 * @ref args::core::async::readonly_rw_spinlock
	 */
	class readonly_guard final
	{
	private:
		readonly_rw_spinlock& m_lock;

	public:
		/**@brief Creates readonly guard and locks for Read-only.
		 */
		readonly_guard(readonly_rw_spinlock& lock) : m_lock(lock)
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
	 * @ref args::core::async::readonly_rw_spinlock
	 */
	class readonly_multiguard
	{
	private:
		std::vector<readonly_rw_spinlock*> m_locks;

	public:
		/**@brief Creates readonly multi-guard and locks for Read-only.
		 */
		template<typename lock_type1 = readonly_rw_spinlock, typename lock_type2 = readonly_rw_spinlock, typename... lock_typesN>
		readonly_multiguard(lock_type1& lock1, lock_type2& lock2, lock_typesN&... locks)
		{
			m_locks.push_back(&lock1);
			m_locks.push_back(&lock2);
			(m_locks.push_back(&locks), ...);

			sparse_set<uint> unlockedLocks;
			for (uint i = 0; i < m_locks.size(); i++)
				unlockedLocks.insert(i);

			bool locked = true;
			do
			{
				locked = true;
				for (uint i : unlockedLocks)
				{
					if (m_locks[i]->try_lock(read))
						unlockedLocks.erase(i);
					else
						locked = false;
				}
			} while (!locked);
		}

		readonly_multiguard(const readonly_multiguard&) = delete;

		/**@brief RAII style unlocks lock from Read-only.
		 */
		~readonly_multiguard()
		{
			for (readonly_rw_spinlock* lock : m_locks)
				lock->unlock(read);
		}

		readonly_multiguard& operator=(readonly_multiguard&&) = delete;
	};

	/**@class readwrite_guard
	 * @brief RAII guard that uses ::async::readonly_rw_spinlock to lock for read-write.
	 * @note Read-Write operations cannot happen simultaneously and will wait for each other.
	 *		 Read-Write operations will also wait for any Read-only operations to be finished.
	 * @ref args::core::async::readonly_rw_spinlock
	 */
	class readwrite_guard final
	{
	private:
		readonly_rw_spinlock& m_lock;

	public:
		/**@brief Creates read-write guard and locks for Read-Write.
		 */
		readwrite_guard(readonly_rw_spinlock& lock) : m_lock(lock)
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
	 * @ref args::core::async::readonly_rw_spinlock
	 */
	class readwrite_multiguard
	{
	private:
		std::vector<readonly_rw_spinlock*> m_locks;

	public:
		/**@brief Creates read-write multi-guard and locks for Read-Write.
		 */
		template<typename lock_type1, typename lock_type2, typename... lock_typesN>
		readwrite_multiguard(lock_type1& lock1, lock_type2& lock2, lock_typesN&... locks)
		{
			m_locks.push_back(&lock1);
			m_locks.push_back(&lock2);
			(m_locks.push_back(&locks), ...);

			sparse_set<uint> unlockedLocks;
			for (uint i = 0; i < m_locks.size(); i++)
				unlockedLocks.insert(i);

			bool locked = true;
			do
			{
				locked = true;
				for (uint i : unlockedLocks)
				{
					if (m_locks[i]->try_lock(write))
						unlockedLocks.erase(i);
					else
						locked = false;
				}
			} while (!locked);
		}

		readwrite_multiguard(const readwrite_multiguard&) = delete;

		/**@brief RAII style unlocks lock from Read-Write.
		 */
		~readwrite_multiguard()
		{
			for (readonly_rw_spinlock* lock : m_locks)
				lock->unlock(write);
		}

		readwrite_multiguard& operator=(readwrite_multiguard&&) = delete;
	};

	/**@class mixed_multiguard
	 * @brief RAII guard that uses multiple ::async::readonly_rw_spinlocks to lock them all for user specified permissions. (similar to std::lock)
	 * @note Read-only operations can happen simultaneously without waiting for each other.
	 *		 Read-only operations will only wait for Read-Write operations to be finished.
	 * @note Read-Write operations cannot happen simultaneously and will wait for each other.
	 *		 Read-Write operations will also wait for any Read-only operations to be finished.
	 * @ref args::core::async::readonly_rw_spinlock
	 */
	template<size_type S>
	class mixed_multiguard
	{
	private:
		std::array<readonly_rw_spinlock*, S / 2> m_locks;
		std::array<lock_state, S / 2> m_states;

		template<size_type I, typename... types>
		void fill(readonly_rw_spinlock& lock, lock_state state, types&&... args)
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
		 * @note Argument order should be as follows: (readonly_rw_spinlock&, lock_state, readonly_rw_spinlock&, lock_state, ...)
		 */
		template<typename... types>
		explicit mixed_multiguard(types&&... arguments)
		{
			static_assert(sizeof...(types) % 2 == 0, "Argument order should be (lock, lock-state, lock, lock-state). Argument count should thus be even.");

			fill<sizeof...(types)>(arguments...);

			sparse_set<uint> unlockedLocks;
			for (uint i = 0; i < m_locks.size(); i++)
				unlockedLocks.insert(i);

			bool locked = true;
			do
			{
				locked = true;
				for (uint i : unlockedLocks)
				{
					if (m_locks[i]->try_lock(m_states[i]))
						unlockedLocks.erase(i);
					else
						locked = false;
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

	template<typename... types>
	mixed_multiguard(types...)->mixed_multiguard<sizeof...(types)>;

}