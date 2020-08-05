#pragma once
#include <core/platform/platform.hpp>
#include <core/types/primitives.hpp>
#include <core/containers/containers.hpp>
#include <core/async/transferable_atomic.hpp>

namespace args::core::scheduling
{
	class ARGS_API Scheduler;
	class ARGS_API Process;

	class ARGS_API ProcessChain
	{
	private:
		std::string m_name;
		id_type m_nameHash;
		Scheduler* m_scheduler;
		sparse_map<id_type, Process*> m_processes;
		async::transferable_atomic<bool> m_exit;

		static void threadedRun(ProcessChain* chain);
	public:
		ProcessChain() = default;

		template<size_type charc>
		ProcessChain(const char(&name)[charc], Scheduler* scheduler) : m_name(name), m_nameHash(nameHash<charc>(name)), m_scheduler(scheduler) { }

		bool run();

		id_type id() { return m_nameHash; }

		void exit();

		void runInCurrentThread();

		void addProcess(Process* process);

		void removeProcess(Process* process);
	};
}