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
#include <sstream>
#include <limits>

/**@file scheduler.hpp
 */

namespace args::core::scheduling
{
	/**@class Scheduler
	 * @brief Major engine part that handles the creation and destruction of threads and process chains. Also takes charge of the main program loop.
	 * @note Also handle synchronization.
	 */
	class Scheduler
	{
	private:
		struct thread_error
		{
			std::string message;
			std::thread::id threadId;
		};

		ProcessChain m_localChain;
		async::readonly_rw_spinlock m_processChainsLock;
		sparse_map<id_type, ProcessChain> m_processChains;
		async::readonly_rw_spinlock m_errorsLock;
		std::vector<thread_error> m_errors;

		async::readonly_rw_spinlock m_exitsLock;
		std::vector<std::thread::id> m_exits;

		std::atomic_bool m_requestSync;
		async::ring_sync_lock m_syncLock;

		std::atomic<float> m_timeScale = 1.f;

		inline static async::readonly_rw_spinlock m_threadsLock;
		inline static sparse_map<std::thread::id, std::unique_ptr<std::thread>> m_threads;
		inline static const uint m_maxThreadCount = std::thread::hardware_concurrency() == 0 ? std::numeric_limits<uint>::max() : std::thread::hardware_concurrency();
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

		/**@brief Set global time scale.
		 */
		void setTimeScale(float scale)
		{
			m_timeScale.store(scale, std::memory_order_relaxed);
		}

		/**@brief Get the global time scale.
		 */
		float getTimeScale()
		{
			return m_timeScale.load(std::memory_order_relaxed);
		}

		/**@brief Run main program loop, also starts all process-chains in their own threads.
		 */
		void run()
		{
			{
				async::readonly_guard guard(m_processChainsLock);
				for (ProcessChain& chain : m_processChains)
					chain.run();
			}

			while (true) //TODO(glyn leine): Check for engine exit flag, needs engine event system/event bus.
			{
				{
					async::readwrite_guard guard(m_exitsLock);

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
					async::readwrite_guard guard(m_errorsLock);

					if (m_errors.size())
					{
						for (thread_error& error : m_errors)
						{
							std::cout << error.message << std::endl;
							destroyThread(error.threadId);
						}

						m_errors.clear();

						throw std::logic_error("");
					}
				}

				{
					std::vector<id_type> toRemove;
					
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

				if (m_localChain.id())
					m_localChain.runInCurrentThread();

				if (syncRequested())
					waitForProcessSync();
			}
		}

		/**@brief Create a new thread.
		 * @param function Function to run on the thread.
		 * @param ...args Arguments to pass to the function.
		 * @return bool True if thread was created, false if there are no available threads.
		 */
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

		/**@brief Destroy a thread.
		 * @warning DON'T USE UNLESS YOU KNOW WHAT YOU ARE DOING.
		 */
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

		/**@brief Report an intentional exit from a thread.
		 */
		void reportExit(const std::thread::id& id)
		{
			async::readwrite_guard guard(m_exitsLock);
			m_exits.push_back(id);
		}

		/**@brief Report an unintentional exit from a thread.
		 */
		void reportExitWithError(const std::string& name, const std::thread::id& id, const args::core::exception& exc)
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

		/**@brief Report an unintentional exit from a thread.
		 */
		void reportExitWithError(const std::thread::id& id, const args::core::exception& exc)
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

		/**@brief Report an unintentional exit from a thread.
		 */
		void reportExitWithError(const std::string& name, const std::thread::id& id, const std::exception& exc)
		{
			async::readwrite_guard guard(m_errorsLock);
			std::stringstream ss;

			ss << "Encountered cross thread exception:"
				<< "\n  thread id:\t" << id
				<< "\n  thread name:\t" << name
				<< "\n  message: \t" << exc.what() << '\n';

			m_errors.push_back({ ss.str(), id });
		}

		/**@brief Report an unintentional exit from a thread.
		 */
		void reportExitWithError(const std::thread::id& id, const std::exception& exc)
		{
			async::readwrite_guard guard(m_errorsLock);
			std::stringstream ss;

			ss << "Encountered cross thread exception:"
				<< "\n  thread id:\t" << id
				<< "\n  message: \t" << exc.what() << '\n';

			m_errors.push_back({ ss.str(), id });
		}

		/**@brief Request thread synchronization and wait for that synchronization moment.
		 */
		void waitForProcessSync()
		{
			std::cout << "synchronizing thread: " << std::this_thread::get_id() << std::endl;
			if (std::this_thread::get_id() != m_syncLock.ownerThread())
			{
				m_requestSync.store(true, std::memory_order_relaxed);
				m_syncLock.sync();
			}
			else
			{
				{
					async::readonly_guard guard(m_processChainsLock);
					while (m_syncLock.waiterCount() != m_processChains.size())
						;
				}

				m_syncLock.sync();
				m_requestSync.store(false, std::memory_order_release);
			}
		}

		/**@brief Check if a synchronization has been requested.
		 */
		bool syncRequested() { return m_requestSync.load(std::memory_order_acquire); }

		/**@brief Get pointer to a certain process-chain.
		 */
		template<size_type charc>
		ProcessChain* getChain(const char(&name)[charc])
		{
			id_type id = nameHash<charc>(name);
			async::readonly_guard guard(m_processChainsLock);
			if (m_processChains.contains(id))
				return &m_processChains.get(id);
			return nullptr;
		}

		/**@brief Create a new process-chain.
		 */
		template<size_type charc>
		ProcessChain* addChain(const char(&name)[charc])
		{
			if (!m_localChain.id())
			{
				m_localChain = std::move(ProcessChain(name, this));
				return &m_localChain;
			}

			id_type id = nameHash<charc>(name);
			async::readwrite_guard guard(m_processChainsLock);
			return &(*m_processChains.emplace(id, name, this).first);
		}


		/**@brief Hook a process to a certain chain.
		 * @return bool True if succeeded, false if the chain doesn't exist.
		 */
		template<size_type charc>
		bool hookProcess(const char(&chainName)[charc], Process* process)
		{
			id_type chainId = nameHash<charc>(chainName);
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

		/**@brief Hook a process to a certain chain.
		 * @return bool True if succeeded, false if the chain doesn't exist.
		 */
		bool hookProcess(cstring chainName, Process* process)
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

		/**@brief Unhook a process from a certain chain.
		 * @return bool True if succeeded, false if the chain doesn't exist.
		 */
		template<size_type charc>
		bool unhookProcess(const char(&chainName)[charc], Process* process)
		{
			id_type chainId = nameHash<charc>(chainName);
			async::readonly_guard guard(m_processChainsLock);
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

		/**@brief Unhook a process from a certain chain.
		 * @return bool True if succeeded, false if the chain doesn't exist.
		 */
		bool unhookProcess(cstring chainName, Process* process)
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
				m_localChain.addProcess(process);
				return true;
			}

			return false;
		}
	};
}