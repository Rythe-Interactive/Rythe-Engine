#pragma once
#include <core/platform/platform.hpp>
#include <core/types/primitives.hpp>
#include <core/types/type_util.hpp>
#include <core/ecs/ecsregistry.hpp>
#include <core/scheduling/scheduler.hpp>
#include <memory>

namespace args::core
{
	class SystemBase
	{
		friend class Module;
	protected:
		ecs::EcsRegistry* m_ecs;
		scheduling::Scheduler* m_scheduler;

		sparse_map<id_type, std::unique_ptr<scheduling::Process>> m_processes;

	public:
		const id_type id;
		const std::string name;

		SystemBase(id_type id, const std::string& name) : id(id), name(name) {}

		virtual void setup() ARGS_PURE;

		virtual ~SystemBase() = default;
	};

	template<typename SelfType>
	class System : public SystemBase
	{
	protected:
		template <void(SelfType::* func_type)(time::time_span<fast_time>), size_type charc>
		void createProcess(const char(&chainName)[charc], time::time_span<fast_time> interval = 0)
		{
			std::string name = std::string(chainName) + undecoratedTypeName<SelfType>() + std::to_string(interval);
			id_type id = nameHash(name);

			std::unique_ptr<scheduling::Process> process = std::make_unique<scheduling::Process>(name, id, interval);
			process->setOperation(delegate<void(time::time_span<fast_time>)>::create<SelfType, func_type>((SelfType*)this));
			m_processes.insert(id, std::move(process));

			m_scheduler->hookProcess<charc>(chainName, m_processes[id].get());
		}

		void createProcess(cstring chainName, delegate<void(time::time_span<fast_time>)>&& operation, time::time_span<fast_time> interval = 0)
		{
			std::string name = std::string(chainName) + undecoratedTypeName<SelfType>() + std::to_string(interval);
			id_type id = nameHash(name);

			std::unique_ptr<scheduling::Process> process = std::make_unique<scheduling::Process>(name, id, interval);
			process->setOperation(operation);
			m_processes.insert(id, std::move(process));

			m_scheduler->hookProcess(chainName, m_processes[id].get());
		}

	public:
		System() : SystemBase(typeHash<SelfType>(), undecoratedTypeName<SelfType>()) {}
		virtual ~System() = default;
	};
}