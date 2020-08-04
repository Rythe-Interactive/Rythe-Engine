#pragma once
#include <core/platform/platform.hpp>
#include <core/types/types.hpp>
#include <core/containers/containers.hpp>
#include <core/time/time.hpp>

namespace args::core::scheduling
{
	class ARGS_API Process
	{
		friend class ProcessChain;
	private:
		const std::string m_name;
		const id_type m_nameHash;
		hashed_sparse_set<id_type> m_hooks;

		delegate<void(time::time_span<fast_time>)> m_operation;
		time::time_span<fast_time> m_interval;
		time::time_span<fast_time> m_timeBuffer;
		time::clock<fast_time> m_clock;
		bool m_fixedTimeStep;
	public:

		template<size_type charc>
		Process(const char(&name)[charc], time::time_span<fast_time> interval = 0) : m_name(name), m_nameHash(nameHash<charc>(name)) { setInterval(interval); }

		id_type id() const { return m_nameHash; }

		void setOperation(delegate<void(time::time_span<fast_time>)>&& operation);

		void setInterval(time::time_span<fast_time> interval);

		bool execute(float timeScale);
	};
}