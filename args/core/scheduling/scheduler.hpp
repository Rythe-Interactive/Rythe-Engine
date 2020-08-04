#pragma once
#include <core/platform/platform.hpp>
#include <core/types/types.hpp>
#include <core/containers/containers.hpp>
#include <core/scheduling/processchain.hpp>

namespace args::core::scheduling
{
	class ARGS_API Scheduler
	{
	private:
		sparse_map<id_type, ProcessChain> m_processChains;

	public:
		void init();

		void run();

		void waitForProcessSync();

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
			id_type id = nameHash<charc>(name);
			return &m_processChains.emplace(id, name, id, this);
		}
	};
}