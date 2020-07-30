#include <core/async/readonly_rw_spinlock.hpp>

namespace args::core::async
{
	std::atomic_uint readonly_rw_spinlock::lastId;

	int& readonly_rw_spinlock::localReaders()
	{
		static thread_local std::unordered_map<uint, int> vals;

		return vals[id];
	}

	int& readonly_rw_spinlock::localWriters()
	{
		static thread_local std::unordered_map<uint, int> vals;

		return vals[id];
	}

	read_state& readonly_rw_spinlock::localState()
	{
		static thread_local std::unordered_map<uint, read_state> states;
		return states[id];
	}

	void readonly_rw_spinlock::read_lock()
	{
		// Report another reader to the lock.
		readers.fetch_add(1, std::memory_order_relaxed);
		localReaders()++;

		if (localState() != read_state::idle) // If we're either already reading or writing then the lock doesn't need to be reacquired.
		{
			return;
		}

		// Expect idle as default.
		int state = read_state::idle;

		// Try to set the lock state to read
		while (!readState.compare_exchange_weak(state, read_state::read, std::memory_order_acquire, std::memory_order_relaxed))
		{
			// If the lock state was already on read we can continue without issues, read-only operations are allowed to happen simultaneously.
			if (state == read_state::read)
				break;
			else // If the lock was in any other state than idle or read then we need to stay in the CAS loop to prevent writes from happening during our read.
				state = read_state::idle;
		}

		localState() = read_state::read; // Set thread_local state to read.
	}

	bool readonly_rw_spinlock::read_try_lock()
	{
		if (localState() != read_state::idle) // If we're either already reading or writing then the lock doesn't need to be reacquired.
		{
			readers.fetch_add(1, std::memory_order_relaxed);
			localReaders()++;
			return true;
		}

		// Expect idle as default.
		int state = read_state::idle;

		// Try to set the lock state to read
		if (!readState.compare_exchange_weak(state, read_state::read, std::memory_order_acquire, std::memory_order_relaxed))
		{
			// If the lock state was already on read we can continue without issues, read-only operations are allowed to happen simultaneously.
			if (state != read_state::read)
				return false;
		}

		readers.fetch_add(1, std::memory_order_relaxed);
		localReaders()++;
		localState() = read_state::read; // Set thread_local state to read.
		return true;
	}

	void readonly_rw_spinlock::write_lock()
	{
		localWriters()++;

		if (localState() == read_state::read) // If we're currently only acquired for read we need to stop reading before requesting rw.
		{
			// Mark our read as finished.
			readers.fetch_sub(1, std::memory_order_relaxed);

			// If there are no more readers left then the lock state needs to be returned to idle.
			// This allows other (non readonly)locks to acquire access.
			if (readers.load(std::memory_order_acquire) == 0)
				readState.store(read_state::idle, std::memory_order_release);
		}
		else if (localState() == read_state::write) // If we're already writing then we don't need to reacquire the lock.
		{
			return;
		}

		// Expect idle as default.
		int state = read_state::idle;

		// Try to set the lock state to write.
		while (!readState.compare_exchange_weak(state, read_state::write, std::memory_order_acquire, std::memory_order_relaxed))
			// If lock state is any other state than idle then we cannot acquire access to write, thus we need to stay in the CAS loop.
			state = read_state::idle;


		localState() = read_state::write; // Set thread_local state to write.
	}

	bool readonly_rw_spinlock::write_try_lock()
	{
		if (localState() == read_state::read) // If we're currently only acquired for read we need to stop reading before requesting rw.
		{
			// Mark our read as finished.
			readers.fetch_sub(1, std::memory_order_relaxed);

			// If there are no more readers left then the lock state needs to be returned to idle.
			// This allows other (non readonly)locks to acquire access.
			if (readers.load(std::memory_order_acquire) == 0)
				readState.store(read_state::idle, std::memory_order_release);
		}
		else if (localState() == read_state::write) // If we're already writing then we don't need to reacquire the lock.
		{
			localWriters()++;
			return true;
		}

		// Expect idle as default.
		int state = read_state::idle;

		// Try to set the lock state to write.
		if (!readState.compare_exchange_weak(state, read_state::write, std::memory_order_acquire, std::memory_order_relaxed))
		{
			if (localReaders() > 0)
			{
				readers.fetch_add(1, std::memory_order_relaxed);
				state = read_state::idle;
				// Try to set the lock state to read
				while (!readState.compare_exchange_weak(state, read_state::read, std::memory_order_acquire, std::memory_order_relaxed))
				{
					// If the lock state was already on read we can continue without issues, read-only operations are allowed to happen simultaneously.
					if (state == read_state::read)
						break;
					else // If the lock was in any other state than idle or read then we need to stay in the CAS loop to prevent writes from happening during our read.
						state = read_state::idle;
				}

				localState() = read_state::read;
			}
			return false;
		}

		localWriters()++;
		localState() = read_state::write; // Set thread_local state to write.
		return true;
	}

	void readonly_rw_spinlock::read_unlock()
	{
		localReaders()--;
		// Mark our read as finished.
		readers.fetch_sub(1, std::memory_order_relaxed);

		if (localReaders() > 0 || localWriters() > 0) // Another local guard is still alive that will unlock the lock for this thread.
		{
			return;
		}

		// If there are no more readers left then the lock state needs to be returned to idle.
		// This allows other (non readonly)locks to acquire access.
		if (readers.load(std::memory_order_acquire) == 0)
			readState.store(read_state::idle, std::memory_order_release);

		localState() = read_state::idle; // Set thread_local state to idle.
	}

	void readonly_rw_spinlock::write_unlock()
	{
		localWriters()--;
		if (localWriters() > 0) // Another write guard is still alive that will unlock the lock for this thread.
		{
			return;
		}
		else if (localReaders() > 0) // read permission was granted before write request, we should return to read instead of idle after write is finished.
		{
			// We should be the only one to have had access so we can safely write to the lock state and readers.
			readers.fetch_add(1, std::memory_order_relaxed);
			readState.store(read_state::read, std::memory_order_release);

			localState() = read_state::read; // Set thread_local state back to read.
		}
		else
		{
			// We should be the only one to have had access so we can safely write to the lock state and return it to idle.
			readState.store(read_state::idle, std::memory_order_release);

			localState() = read_state::idle; // Set thread_local state to idle.
		}
	}

	readonly_rw_spinlock::readonly_rw_spinlock() : id(lastId.fetch_add(1, std::memory_order_relaxed))
	{
		localState() = read_state::idle;

		readState.store(read_state::idle, std::memory_order_relaxed);
		readers.store(0, std::memory_order_relaxed);
	}

	inline void readonly_rw_spinlock::lock(read_state permissionLevel)
	{
		switch (permissionLevel)
		{
		case read_state::read:
			return read_lock();
		case read_state::write:
			return write_lock();
		default:
			return;
		}
	}

	inline bool readonly_rw_spinlock::try_lock(read_state permissionLevel)
	{
		switch (permissionLevel)
		{
		case read_state::read:
			return read_try_lock();
		case read_state::write:
			return write_try_lock();
		default:
			return false;
		}
	}

	inline void readonly_rw_spinlock::unlock(read_state permissionLevel)
	{
		switch (permissionLevel)
		{
		case args::core::async::read_state::read:
			return read_unlock();
		case args::core::async::read_state::write:
			return write_unlock();
		default:
			return;
		}
	}
}