#pragma once
#include <core/platform/platform.hpp>
#include <core/types/primitives.hpp>
#include <core/containers/containers.hpp>
#include <core/time/time_span.hpp>

namespace args::core::scheduling
{
	class ARGS_API Process
	{
	private:
		static id_type m_lastId;
		std::string m_name;
		id_type m_nameHash;
		hashed_sparse_set<id_type> m_hooks;

		delegate<void(fast_time)> m_operation;
		time::time_span<> m_interval;
		time::time_span<> m_timeBuffer;

	public:
		void setOperation(delegate<void(fast_time)>&& operation);

		void setInterval(fast_seconds interval);

		bool execute(fast_seconds deltaTime);
	};
}