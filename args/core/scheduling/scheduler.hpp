#pragma once
#include <core/platform/platform.hpp>
#include <core/types/types.hpp>
#include <core/containers/containers.hpp>
#include <core/scheduling/processchain.hpp>
#include <core/async/async.hpp>
#include <core/common/exception.hpp>
#include <core/events/events.hpp>

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
	class ARGS_API Scheduler
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

		events::EventBus* m_eventBus;

		static async::readonly_rw_spinlock m_threadsLock;
		static sparse_map<std::thread::id, std::unique_ptr<std::thread>> m_threads;
		static const uint m_maxThreadCount;
		static async::readonly_rw_spinlock m_availabilityLock;
		static uint m_availableThreads;

	public:
		Scheduler(events::EventBus* eventBus);		

		~Scheduler();

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
		void run();

		/**@brief Create a new thread.
		 * @param function Function to run on the thread.
		 * @param ...args Arguments to pass to the function.
		 * @return std::thread::id Id of new thread if thread was created, 0 id if there are no available threads.
		 */
		template<typename Function, typename... Args >
		std::thread::id createThread(Function&& function, Args&&... args)
		{
			async::readwrite_multiguard guard(m_availabilityLock, m_threadsLock);

			if (m_availableThreads) // Check if there are available threads.
			{
				m_availableThreads--;
				std::unique_ptr<std::thread> newThread = std::make_unique<std::thread>(function, args...); // Create a new thread and run it.
				std::thread::id id = newThread->get_id();
				m_threads.insert(id, std::move(newThread));
				return id;
			}

			return std::thread::id();
		}

		/**@brief Destroy a thread.
		 * @warning DON'T USE UNLESS YOU KNOW WHAT YOU ARE DOING.
		 */
		void destroyThread(std::thread::id id);

		/**@brief Report an intentional exit from a thread.
		 */
		void reportExit(const std::thread::id& id);

		/**@brief Report an unintentional exit from a thread.
		 */
		void reportExitWithError(const std::string& name, const std::thread::id& id, const args::core::exception& exc);

		/**@brief Report an unintentional exit from a thread.
		 */
		void reportExitWithError(const std::thread::id& id, const args::core::exception& exc);

		/**@brief Report an unintentional exit from a thread.
		 */
		void reportExitWithError(const std::string& name, const std::thread::id& id, const std::exception& exc);

		/**@brief Report an unintentional exit from a thread.
		 */
		void reportExitWithError(const std::thread::id& id, const std::exception& exc);		

		/**@brief Request thread synchronization and wait for that synchronization moment.
		 */
		void waitForProcessSync();

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
		ProcessChain* addProcessChain(const char(&name)[charc])
		{
			if (!m_localChain.id())
			{
				m_localChain = ProcessChain(name, this);
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
		bool hookProcess(const char(&processChainName)[charc], Process* process)
		{
			id_type chainId = nameHash<charc>(processChainName);
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
		bool hookProcess(cstring chainName, Process* process);
		

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
				m_localChain.removeProcess(process);
				return true;
			}

			return false;
		}

		/**@brief Unhook a process from a certain chain.
		 * @return bool True if succeeded, false if the chain doesn't exist.
		 */
		bool unhookProcess(cstring chainName, Process* process);
		
	};
}
