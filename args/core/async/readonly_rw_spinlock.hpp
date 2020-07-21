#pragma once
#include <atomic>
#include <mutex>

namespace args::core::async
{
	struct readonly_rw_spinlock
	{
		friend class readonly_guard;
		friend class readwrite_guard;
	private:
		enum read_state { idle = 0, read = 1, write = 2 };

		std::atomic_int readState;
		std::atomic_int readers;
	};

	class readonly_guard final
	{
	private:
		readonly_rw_spinlock& lock;

	public:
		readonly_guard(readonly_rw_spinlock& lock);
		readonly_guard(const readonly_guard&) = delete;

		~readonly_guard();

		readonly_guard& operator=(readonly_guard&&) = delete;
	};

	class readwrite_guard final
	{
	private:
		readonly_rw_spinlock& lock;

	public:
		readwrite_guard(readonly_rw_spinlock& lock);
		readwrite_guard(const readwrite_guard&) = delete;

		~readwrite_guard();

		readwrite_guard& operator=(readwrite_guard&&) = delete;
	};
}