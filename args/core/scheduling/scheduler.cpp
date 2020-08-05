#include <core/scheduling/scheduler.hpp>
#include <iostream>

namespace args::core::scheduling
{
	async::readonly_rw_spinlock Scheduler::m_threadsLock;
	sparse_map<std::thread::id, std::thread> Scheduler::m_threads;
	async::readonly_rw_spinlock Scheduler::m_availabilityLock;

	void Scheduler::init()
	{
		addChain("Update");
	}

	void Scheduler::run()
	{
		m_syncLock.lock(async::write);

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
			{
				m_localChain.runInCurrentThread();
			}

			bool sync;

			{
				async::readonly_guard guard(m_requestLock);
				sync = m_requestSync;				
			}

			if (sync)
			{
				bool waiting = true;
				while (waiting)
				{
					async::readonly_guard guard(m_waitLock);
					if (m_waitingThreads.size() == m_processChains.size())
						waiting = false;
				}
				/// Needs rethinking, if we relock the sync-lock before a process was able to advance they'll be stuck. we need to keep track of which lock iteration the block is part of.
			}
		}
	}

	void Scheduler::waitForProcessSync()
	{
		{
			async::readwrite_guard guard(m_requestLock);
			m_requestSync = true;
		}

		{
			async::readwrite_guard guard(m_waitLock);
			m_waitingThreads.push_back(std::this_thread::get_id());
		}

		async::readwrite_guard syncGuard(m_syncLock);
	}
}