#pragma once
#include <atomic>
#include <tuple>
#include <unordered_map>
#include <core/types/primitives.hpp>
#include <core/platform/platform.hpp>
#include <core/containers/sparse_set.hpp>

/**
 * @file readonly_rw_spinlock.hpp
 */

namespace args::core::async
{
	enum read_state { idle = 0, read = 1, write = 2 };

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
	private:
		static std::atomic_uint lastId;
		const uint id;
		std::atomic_int readState;
		std::atomic_int readers;

		int& localWriters();
		int& localReaders();
		read_state& localState();

		void read_lock();
		bool read_try_lock();
		void write_lock();
		bool write_try_lock();

		void read_unlock();
		void write_unlock();

	public:
		readonly_rw_spinlock();
		readonly_rw_spinlock(const readonly_rw_spinlock&) = delete;
		readonly_rw_spinlock& operator=(readonly_rw_spinlock&&) = delete;

		void lock(read_state permissionLevel);
		bool try_lock(read_state permissionLevel);
		void unlock(read_state permissionLevel);
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
	 * @brief .
	 * @ref args::core::async::readonly_rw_spinlock
	 */
	template<typename lock_type1 = readonly_rw_spinlock, typename lock_type2 = readonly_rw_spinlock, typename... lock_typesN>
	class readonly_multiguard
	{
	private:
		std::vector<readonly_rw_spinlock*> m_locks;

	public:
		/**@brief Creates readonly multi-guard and locks for Read-only.
		 */
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
	 * @brief RAII guard that uses ::async::readonly_rw_spinlock to lock for read/write.
	 *        Read-Write operations cannot happen simultaneously and will wait for each other.
	 *		  Read-Write operations will also wait for any Read-only operations to be finished.
	 * @ref args::core::async::readonly_rw_spinlock
	 */
	class readwrite_guard final
	{
	private:
		readonly_rw_spinlock& m_lock;

	public:
		/**@brief Creates readonly guard and locks for Read-Write.
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
	 * @brief .
	 * @ref args::core::async::readonly_rw_spinlock
	 */
	template<typename lock_type1, typename lock_type2, typename... lock_typesN>
	class readwrite_multiguard
	{
	private:
		std::vector<readonly_rw_spinlock*> m_locks;

	public:
		/**@brief Creates readonly multi-guard and locks for Read-Write.
		 */
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
	 * @brief .
	 * @ref args::core::async::readonly_rw_spinlock
	 */
	template<typename... mixed_types>
	class mixed_multiguard
	{
		static_assert(sizeof...(mixed_types) % 2 == 0, "Argument order should be (lock, lock-state, lock, lock-state). Argument count should thus be even.");

	private:
		std::vector<readonly_rw_spinlock*> m_locks;
		std::vector<read_state*> m_states;

		template<size_type I>
		void fillVectors(std::tuple<mixed_types*...>& args)
		{
			if (I - 1 >= 1)
			{
				fillVectors<I - 2>(args);

				m_locks.push_back(std::get<I-1>(args));
				m_states.push_back(std::get<I>(args));
			}
		}

		template<>
		void fillVectors<1>(std::tuple<mixed_types*...>& args)
		{
			m_locks.push_back(std::get<0>(args));
			m_states.push_back(std::get<1>(args));
		}

	public:
		/**@brief Creates readonly multi-guard and locks for Read-Write.
		 */
		mixed_multiguard(mixed_types&... arguments)
		{
			std::tuple<mixed_types*...> argsdata = std::make_tuple(&arguments...);

			fillVectors<sizeof...(mixed_types)-1>(argsdata);

			sparse_set<uint> unlockedLocks;
			for (uint i = 0; i < m_locks.size(); i++)
				unlockedLocks.insert(i);

			bool locked = true;
			do
			{
				locked = true;
				for (uint i : unlockedLocks)
				{
					if (m_locks[i]->try_lock(*(m_states[i])))
						unlockedLocks.erase(i);
					else
						locked = false;
				}
			} while (!locked);
		}

		mixed_multiguard(const mixed_multiguard&) = delete;

		/**@brief RAII style unlocks lock from Read-Write.
		 */
		~mixed_multiguard()
		{
			for(int i = 0; i < m_locks.size(); i++)
				m_locks[i]->unlock(*(m_states[i]));
		}

		mixed_multiguard& operator=(mixed_multiguard&&) = delete;
	};

}