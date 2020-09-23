#pragma once
#include <core/platform/platform.hpp>
#include <core/types/primitives.hpp>
#include <core/types/type_util.hpp>
#include <core/containers/containers.hpp>
#include <core/async/transferable_atomic.hpp>

#include <thread>

/**@file processchain.hpp
 */

namespace args::core::scheduling
{
	class ARGS_API Scheduler;
	class ARGS_API Process;

	/**@class ProcessChain
	 * @brief Chain of processes that either run in a separate thread or on the main thread.
	 * @note If chain is to run on a separate thread, then the chain has it's own program loop. ProcessChain::exit() needs to be called in order to end the thread.
	 */
	class ARGS_API ProcessChain
	{
	private:
		std::string m_name;
		id_type m_nameHash = invalid_id;
		std::thread::id m_threadId;
		Scheduler* m_scheduler;
		async::readonly_rw_spinlock m_processesLock;
		sparse_map<id_type, Process*> m_processes;
		async::transferable_atomic<bool> m_exit;
        bool m_low_power;

	public:
        static void threadedRun(ProcessChain* chain);

		ProcessChain() = default;
		ProcessChain(ProcessChain&&) = default;
		ProcessChain& operator=(ProcessChain&&) = default;

		template<size_type charc>
		ProcessChain(const char(&name)[charc], Scheduler* scheduler) : m_name(name), m_nameHash(nameHash<charc>(name)), m_scheduler(scheduler) { }

		/**@brief Creates a new thread and runs it's own program loop in it.
		 * @return bool Scheduler::createThread()
		 * @ref Scheduler::createThread()
		 */
		bool run(bool low_power);

		/**@brief Returns the hash of the name of the process-chain.
		 */
		id_type id() { return m_nameHash; }


		/**@brief Get id of the thread the chain runs on.
		 */
		std::thread::id threadId() 
		{
			if (m_threadId == std::thread::id())
				return std::this_thread::get_id();
			return m_threadId; 
		}

		/**@brief Raises exit flag for this process-chain and will request the process thread to exit.
		 */
		void exit();

		/**@brief Runs one iteration of the process-chains program loop without creating a new thread.
		 * @note Loops through all hooked processes and executes them until they are all finished.
		 */
		void runInCurrentThread();

		/**@brief Hook a process for execution with this chain.
		 */
		void addProcess(Process* process);

		/**@brief Unhook a process from execution with this chain.
		 */
		void removeProcess(Process* process);
	};
}
