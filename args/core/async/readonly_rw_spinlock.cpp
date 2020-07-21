#include "readonly_rw_spinlock.hpp"

args::core::async::readonly_guard::readonly_guard(readonly_rw_spinlock& lock) : lock(lock)
{
	int state = readonly_rw_spinlock::idle;
	lock.readers.fetch_add(1, std::memory_order_relaxed);
	while (!lock.readState.compare_exchange_weak(state, readonly_rw_spinlock::read, std::memory_order_acquire, std::memory_order_relaxed))
	{
		if (state == readonly_rw_spinlock::read)
			break;
		else 
			state = readonly_rw_spinlock::idle;
	}
}

args::core::async::readonly_guard::~readonly_guard()
{
	lock.readers.fetch_sub(1, std::memory_order_relaxed);

	if (lock.readers.load(std::memory_order_acquire) == 0)
		lock.readState.store(readonly_rw_spinlock::idle, std::memory_order_release);
}

args::core::async::readwrite_guard::readwrite_guard(readonly_rw_spinlock& lock) : lock(lock)
{
	int state = readonly_rw_spinlock::idle;
	while (!lock.readState.compare_exchange_weak(state, readonly_rw_spinlock::write, std::memory_order_acquire, std::memory_order_relaxed))
		state = readonly_rw_spinlock::idle;
}

args::core::async::readwrite_guard::~readwrite_guard()
{
	lock.readState.store(readonly_rw_spinlock::idle, std::memory_order_release);
}
