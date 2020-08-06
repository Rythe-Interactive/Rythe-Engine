#include <core/scheduling/scheduler.hpp>
#include <iostream>

namespace args::core::scheduling
{
	async::readonly_rw_spinlock Scheduler::m_threadsLock;
	sparse_map<std::thread::id, std::thread*> Scheduler::m_threads;
	async::readonly_rw_spinlock Scheduler::m_availabilityLock;	

	void Scheduler::run()
	{
		for (ProcessChain& chain : m_processChains)
			chain.run();

		while (true) // check for engine exit flag
		{
			{
				async::readwrite_guard guard(m_errorsLock);

				if (m_errors.size())
				{
					for (thread_error& error : m_errors)
					{
						std::cout << error.message << std::endl;
						destroyThread(error.threadId);
					}

					m_errors.clear();

					throw args_exception_msg("An exception occurred in a different thread.");
				}
			}

			if (m_localChain.id())
				m_localChain.runInCurrentThread();

			if (m_syncLock.waiterCount() == m_processChains.size())
			{
				m_syncLock.sync();
				m_requestSync.store(false, std::memory_order_release);
			}
		}
	}

	void Scheduler::waitForProcessSync()
	{
		if (std::this_thread::get_id() != m_syncLock.ownerThread())
		{
			m_requestSync.store(true, std::memory_order_relaxed);
			m_syncLock.sync();
		}
		else
			while (m_syncLock.waiterCount() == m_processChains.size())
				;
	}
}