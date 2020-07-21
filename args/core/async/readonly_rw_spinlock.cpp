#include "readonly_rw_spinlock.hpp"

args::core::async::readonly_guard::readonly_guard(readonly_rw_spinlock& lock) : lock(lock)
{
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
}

args::core::async::readonly_guard::~readonly_guard()
{
	// Mark our read as finished.
	lock.readers.fetch_sub(1, std::memory_order_relaxed);

	// If there are no more readers left then the lock state needs to be returned to idle.
	// This allows other (non readonly)locks to acquire access.
	if (lock.readers.load(std::memory_order_acquire) == 0)
		lock.readState.store(readonly_rw_spinlock::idle, std::memory_order_release);
}

args::core::async::readwrite_guard::readwrite_guard(readonly_rw_spinlock& lock) : lock(lock)
{
	// Expect idle as default.
	int state = readonly_rw_spinlock::idle;

	// Try to set the lock state to write.
	while (!lock.readState.compare_exchange_weak(state, readonly_rw_spinlock::write, std::memory_order_acquire, std::memory_order_relaxed))
		// If lock state is any other state than idle then we cannot acquire access to write, thus we need to stay in the CAS loop.
		state = readonly_rw_spinlock::idle;
}

args::core::async::readwrite_guard::~readwrite_guard()
{
	// We should be the only one to have had access so we can safely write to the lock state and return it to idle.
	lock.readState.store(readonly_rw_spinlock::idle, std::memory_order_release);
}
