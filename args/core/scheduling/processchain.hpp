#pragma once
#include <core/platform/platform.hpp>
#include <core/types/primitives.hpp>
#include <core/containers/containers.hpp>

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

	public:

		void run();

		void runInCurrentThread();

		void addProcess(Process* process);

		void removeProcess(Process* process);
	};
}