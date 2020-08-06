#include <core/scheduling/processchain.hpp>
#include <core/scheduling/process.hpp>
#include <core/scheduling/scheduler.hpp>
#include <core/common/exception.hpp>
#include <thread>

namespace args::core::scheduling
{
	void ProcessChain::threadedRun(ProcessChain* chain)
	{
		try
		{
			chain->m_threadId = std::this_thread::get_id();
			while (!chain->m_exit->load(std::memory_order_acquire)) // check for exit flag
			{
				chain->runInCurrentThread();
				if (chain->m_scheduler->syncRequested())
					chain->m_scheduler->waitForProcessSync();
			}

			chain->m_scheduler->reportExit(chain->m_threadId);
		}
		catch (const args::core::exception& e)
		{
			chain->m_scheduler->reportExitWithError(chain->m_name, std::this_thread::get_id(), e);
		}
		catch (const std::exception& e)
		{
			chain->m_scheduler->reportExitWithError(chain->m_name, std::this_thread::get_id(), e);
		}
	}

	inline bool ProcessChain::run()
	{
		m_exit->store(false, std::memory_order_release);
		return m_scheduler->createThread(threadedRun, this);
	}

	inline void ProcessChain::exit()
	{
		m_exit->store(true, std::memory_order_release);
	}

	inline void ProcessChain::runInCurrentThread()
	{
		hashed_sparse_set<id_type> finishedProcesses;
		async::readonly_guard guard(m_processesLock);
		do
		{
			for (auto process : m_processes)
				if (!finishedProcesses.contains(process->id()))
					if (process->execute(1.f)) ///TODO(glyn leine): get time scale from engine somewhere
						finishedProcesses.insert(process->id());
		} while (finishedProcesses.size() != m_processes.size());
	}

	inline void ProcessChain::addProcess(Process* process)
	{
		async::readwrite_guard guard(m_processesLock);
		if (m_processes.insert(process->id(), process).second)
			process->m_hooks.insert(m_nameHash);
	}

	inline void ProcessChain::removeProcess(Process* process)
	{
		async::readwrite_guard guard(m_processesLock);
		if (m_processes.erase(process->id()))
			process->m_hooks.erase(m_nameHash);
	}
}