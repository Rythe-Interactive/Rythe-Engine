#include <core/async/readonly_rw_spinlock.hpp>

namespace args::core::async
{
	std::atomic_uint readonly_rw_spinlock::lastId;

	std::unordered_map<uint, int>& readonly_rw_spinlock::localStates()
	{
		static thread_local std::unordered_map<uint, int> states;
		return states;
	}

	readonly_rw_spinlock::readonly_rw_spinlock() : id(lastId.fetch_add(1, std::memory_order_relaxed))
	{
		localStates()[id] = read_state::idle;

		readState.store(0, std::memory_order_relaxed);
		readers.store(0, std::memory_order_relaxed);
	}
}