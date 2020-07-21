#pragma once
#include <atomic>

/**
 * @file readonly_rw_spinlock.hpp
 */

namespace args::core::async
{
	/**@class readonly_rw_spinlock
	 * @brief Lock used with ::async::readonly_guard and ::async::readwrite_guard.
	 * @ref args::core::async::readonly_guard
	 * @ref args::core::async::readwrite_guard
	 */
	struct readonly_rw_spinlock
	{
		friend class readonly_guard;
		friend class readwrite_guard;
	private:
		enum read_state { idle = 0, read = 1, write = 2 };

		std::atomic_int readState;
		std::atomic_int readers;
	};

	/**@class readonly_guard
	 * @brief RAII guard that uses ::async::readonly_rw_spinlock to lock for read-only.
	 *        Read-only operations can happen simultaneously without waiting for eachother.
	 *		  Read-only operations will only wait for Read-Write operations to be finished.
	 * @ref args::core::async::readonly_rw_spinlock
	 */
	class readonly_guard final
	{
	private:
		readonly_rw_spinlock& lock;

	public:
		/**@brief Creates readonly guard and locks for Read-only.
		 */
		readonly_guard(readonly_rw_spinlock& lock);
		readonly_guard(const readonly_guard&) = delete;

		/**@brief RAII style unlocks lock from Read-only.
		 */
		~readonly_guard();

		readonly_guard& operator=(readonly_guard&&) = delete;
	};

	/**@class readwrite_guard
	 * @brief RAII guard that uses ::async::readonly_rw_spinlock to lock for read/write.
	 *        Read-Write operations cannot happen simultaneously and will wait for eachother.
	 *		  Read-Write operations will also wait for any Read-only operations to be finished.
	 * @ref args::core::async::readonly_rw_spinlock
	 */
	class readwrite_guard final
	{
	private:
		readonly_rw_spinlock& lock;

	public:
		/**@brief Creates readonly guard and locks for Read-Write.
		 */
		readwrite_guard(readonly_rw_spinlock& lock);
		readwrite_guard(const readwrite_guard&) = delete;

		/**@brief RAII style unlocks lock from Read-Write.
		 */
		~readwrite_guard();

		readwrite_guard& operator=(readwrite_guard&&) = delete;
	};
}