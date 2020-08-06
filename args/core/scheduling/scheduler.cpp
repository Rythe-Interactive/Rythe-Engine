#include <core/scheduling/scheduler.hpp>

namespace args::core::scheduling
{
	async::readonly_rw_spinlock Scheduler::m_threadsLock;
	sparse_map<std::thread::id, std::unique_ptr<std::thread>> Scheduler::m_threads;
	const uint Scheduler::m_maxThreadCount = std::thread::hardware_concurrency() == 0 ? std::numeric_limits<uint>::max() : std::thread::hardware_concurrency();
	async::readonly_rw_spinlock Scheduler::m_availabilityLock;
	uint Scheduler::m_availableThreads = m_maxThreadCount - 2; // subtract OS and this_thread.

	Scheduler::Scheduler()
	{
		addChain("Update");
	}

	Scheduler::~Scheduler()
	{
		for (auto& processChain : m_processChains)
			processChain.exit();

		for (auto& thread : m_threads)
			if (thread->joinable())
				thread->join();
	}

	void Scheduler::run()
	{
		{ // Start threads of all the other chains.
			async::readonly_guard guard(m_processChainsLock);
			for (ProcessChain& chain : m_processChains)
				chain.run();
		}

		while (true) //TODO(glyn leine): Check for engine exit flag, needs engine event system/event bus.
		{
			{
				async::readwrite_guard guard(m_exitsLock); // Check for any intentionally exited threads and clean them up.

				if (m_exits.size())
				{
					for (auto& id : m_exits)
					{
						destroyThread(id);
					}

					m_exits.clear();
				}
			}

			{
				async::readwrite_guard guard(m_errorsLock); // Check for any unintentionally exited threads and clean them up.

				if (m_errors.size())
				{
					for (thread_error& error : m_errors)
					{
						std::cout << error.message << std::endl;
						destroyThread(error.threadId);
					}

					m_errors.clear();

					throw std::logic_error(""); // Re-throw an empty error so that the normal error handling system can take care of the rest.
				}
			}

			{
				std::vector<id_type> toRemove; // Check for all the chains that have exited their threads and remove them from the chain list.

				{
					async::readonly_multiguard rmguard(m_processChainsLock, m_threadsLock);
					for (auto& chain : m_processChains)
						if (!m_threads.contains(chain.threadId()))
							toRemove.push_back(chain.id());
				}

				async::readwrite_guard wguard(m_processChainsLock);
				for (id_type& id : toRemove)
					m_processChains.erase(id);
			}

			if (m_localChain.id()) // If the local chain is valid run an iteration.
				m_localChain.runInCurrentThread();

			if (syncRequested()) // If a major engine sync was requested halt thread until all threads have reached a sync point and let them all continue.
				waitForProcessSync();
		}
	}

	void Scheduler::destroyThread(std::thread::id id)
	{
		async::readwrite_multiguard guard(m_availabilityLock, m_threadsLock);

		if (m_threads.contains(id)) // Check if thread exists.
		{
			m_availableThreads++;
			if (m_threads[id]->joinable()) // If the thread is still running then we need to wait for it to finish.
				m_threads[id]->join();
			m_threads.erase(id); // Remove the thread.
		}

	}

	void Scheduler::reportExit(const std::thread::id& id)
	{
		async::readwrite_guard guard(m_exitsLock);
		m_exits.push_back(id);
	}

	void Scheduler::reportExitWithError(const std::string& name, const std::thread::id& id, const args::core::exception& exc)
	{
		async::readwrite_guard guard(m_errorsLock);
		std::stringstream ss;

		ss << "Encountered cross thread exception:"
			<< "\n  thread id:\t" << id
			<< "\n  thread name:\t" << name
			<< "\n  message: \t" << exc.what()
			<< "\n  file:    \t" << exc.file()
			<< "\n  line:    \t" << exc.line()
			<< "\n  function:\t" << exc.func() << '\n';

		m_errors.push_back({ ss.str(), id });
	}

	void Scheduler::reportExitWithError(const std::thread::id& id, const args::core::exception& exc)
	{
		async::readwrite_guard guard(m_errorsLock);
		std::stringstream ss;

		ss << "Encountered cross thread exception:"
			<< "\n  thread id:\t" << id
			<< "\n  message: \t" << exc.what()
			<< "\n  file:    \t" << exc.file()
			<< "\n  line:    \t" << exc.line()
			<< "\n  function:\t" << exc.func() << '\n';

		m_errors.push_back({ ss.str(), id });
	}

	void Scheduler::reportExitWithError(const std::string& name, const std::thread::id& id, const std::exception& exc)
	{
		async::readwrite_guard guard(m_errorsLock);
		std::stringstream ss;

		ss << "Encountered cross thread exception:"
			<< "\n  thread id:\t" << id
			<< "\n  thread name:\t" << name
			<< "\n  message: \t" << exc.what() << '\n';

		m_errors.push_back({ ss.str(), id });
	}

	void Scheduler::reportExitWithError(const std::thread::id& id, const std::exception& exc)
	{
		async::readwrite_guard guard(m_errorsLock);
		std::stringstream ss;

		ss << "Encountered cross thread exception:"
			<< "\n  thread id:\t" << id
			<< "\n  message: \t" << exc.what() << '\n';

		m_errors.push_back({ ss.str(), id });
	}

	void Scheduler::waitForProcessSync()
	{
		std::cout << "synchronizing thread: " << std::this_thread::get_id() << std::endl;
		if (std::this_thread::get_id() != m_syncLock.ownerThread()) // Check if this is the main thread or not.
		{
			m_requestSync.store(true, std::memory_order_relaxed); // Request a synchronization.
			m_syncLock.sync(); // Wait for synchronization moment.
		}
		else
		{
			{
				async::readonly_guard guard(m_processChainsLock);
				while (m_syncLock.waiterCount() != m_processChains.size()) // Wait until all other threads have reached the synchronization moment.
					;
			}

			m_requestSync.store(false, std::memory_order_release);
			m_syncLock.sync(); // Release sync lock.
		}
	}

	bool Scheduler::hookProcess(cstring chainName, Process* process)
	{
		id_type chainId = nameHash(chainName);
		async::readonly_guard guard(m_processChainsLock);
		if (m_processChains.contains(chainId))
		{
			m_processChains[chainId].addProcess(process);
			return true;
		}
		else if (m_localChain.id() == chainId)
		{
			m_localChain.addProcess(process);
			return true;
		}

		return false;
	}

	bool Scheduler::unhookProcess(cstring chainName, Process* process)
	{
		id_type chainId = nameHash(chainName);
		async::readonly_guard guard(m_processChainsLock);
		if (m_processChains.contains(chainId))
		{
			m_processChains[chainId].removeProcess(process);
			return true;
		}
		else if (m_localChain.id() == chainId)
		{
			m_localChain.removeProcess(process);
			return true;
		}

		return false;
	}

}