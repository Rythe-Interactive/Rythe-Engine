#pragma once
#include <core/platform/platform.hpp>
#include <core/types/types.hpp>
#include <core/containers/containers.hpp>
#include <core/scheduling/processchain.hpp>
#include <core/async/async.hpp>
#include <core/common/exception.hpp>

#include <memory>
#include <thread>
#include <atomic>
#include <iostream>

namespace args::core::scheduling
{
	class Scheduler
	{
	private:
		struct thread_error
		{
			std::string message;
			std::thread::id threadId;
		};

		ProcessChain m_localChain;
		sparse_map<id_type, ProcessChain> m_processChains;
		async::readonly_rw_spinlock m_errorsLock;
		std::vector<thread_error> m_errors;

		std::atomic_bool m_requestSync;
		async::ring_sync_lock m_syncLock;

		inline static async::readonly_rw_spinlock m_threadsLock;
		inline static sparse_map<std::thread::id, std::unique_ptr<std::thread>> m_threads;
		inline static const uint m_maxThreadCount = std::thread::hardware_concurrency() == 0 ? UINT_MAX : std::thread::hardware_concurrency();
		inline static async::readonly_rw_spinlock m_availabilityLock;
		inline static uint m_availableThreads = m_maxThreadCount - 2; // subtract OS and this_thread.

	public:
		Scheduler()
		{
			addChain("Update");
		}

		~Scheduler()
		{
			for (auto& processChain : m_processChains)
				processChain.exit();

			for (auto& thread : m_threads)
				if (thread->joinable())
					thread->join();
		}

		void run()
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

		template<typename Function, typename... Args >
		bool createThread(Function&& function, Args&&... args)
		{
			async::readwrite_multiguard guard(m_availabilityLock, m_threadsLock);

			if (m_availableThreads)
			{
				m_availableThreads--;
				std::unique_ptr<std::thread> newThread = std::make_unique<std::thread>(function, args...);
				m_threads.insert(newThread->get_id(), std::move(newThread));
				return true;
			}

			return false;
		}

		void destroyThread(std::thread::id id)
		{
			async::readwrite_multiguard guard(m_availabilityLock, m_threadsLock);

			if (m_threads.contains(id))
			{
				m_availableThreads++;
				if (m_threads[id]->joinable())
					m_threads[id]->join();
				m_threads.erase(id);
			}

		}

		void reportExitWithError(const std::thread::id& id, const args::core::exception& exc)
		{
			async::readwrite_guard guard(m_errorsLock);
			m_errors.push_back({
				std::string("Encountered exception:\n  msg:     \t") + exc.what() +
				"\n  file:    \t" + exc.file() +
				"\n  line:    \t" + std::to_string(exc.line()) +
				"\n  function:\t" + exc.func() + '\n'
				, id });
		}

		void reportExitWithError(const std::thread::id& id, const std::exception& exc)
		{
			async::readwrite_guard guard(m_errorsLock);
			m_errors.push_back({ std::string("Encountered exception:\n  msg:     \t") + exc.what(), id });
		}

		void waitForProcessSync()
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

		bool syncRequested() { return m_requestSync.load(std::memory_order_acquire); }

		template<size_type charc>
		ProcessChain* getChain(const char(&name)[charc])
		{
			id_type id = nameHash<charc>(name);
			if (m_processChains.contains(id))
				return &m_processChains.get(id);
			return nullptr;
		}

		template<size_type charc>
		ProcessChain* addChain(const char(&name)[charc])
		{
			if (!m_localChain.id())
			{
				m_localChain = ProcessChain(name, this);
				return &m_localChain;
			}

			id_type id = nameHash<charc>(name);
			return &(*m_processChains.emplace(id, name, this).first);
		}

		template<size_type charc>
		bool hookProcess(const char(&chainName)[charc], Process* process)
		{
			id_type chainId = nameHash<charc>(chainName);
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

		bool hookProcess(cstring chainName, Process* process)
		{
			id_type chainId = nameHash(chainName);
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

		template<size_type charc>
		bool unhookProcess(const char(&chainName)[charc], Process* process)
		{
			id_type chainId = nameHash<charc>(chainName);
			if (m_processChains.contains(chainId))
			{
				m_processChains[chainId].removeProcess(process);
				return true;
			}
			else if (m_localChain.id() == chainId)
			{
				m_localChain.addProcess(process);
				return true;
			}

			return false;
		}

		bool unhookProcess(cstring chainName, Process* process)
		{
			id_type chainId = nameHash(chainName);
			if (m_processChains.contains(chainId))
			{
				m_processChains[chainId].removeProcess(process);
				return true;
			}
			else if (m_localChain.id() == chainId)
			{
				m_localChain.addProcess(process);
				return true;
			}

			return false;
		}
	};
}