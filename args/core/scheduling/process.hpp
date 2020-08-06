#pragma once
#include <core/platform/platform.hpp>
#include <core/types/types.hpp>
#include <core/containers/containers.hpp>
#include <core/time/time.hpp>

namespace args::core::scheduling
{
	class Process
	{
		friend class ProcessChain;
	private:
		std::string m_name;
		id_type m_nameHash;
		hashed_sparse_set<id_type> m_hooks;

		delegate<void(time::time_span<fast_time>)> m_operation;
		time::time_span<fast_time> m_interval;
		time::time_span<fast_time> m_timeBuffer;
		time::clock<fast_time> m_clock;
		bool m_fixedTimeStep;
	public:

		template<size_type charc>
		Process(const char(&name)[charc], time::time_span<fast_time> interval = 0) : m_name(name), m_nameHash(nameHash<charc>(name)) { setInterval(interval); }

		Process(const std::string& name, id_type nameHash, time::time_span<fast_time> interval = 0) : m_name(name), m_nameHash(nameHash) { setInterval(interval); }

		Process() = default;
		Process(Process&&) = default;
		Process& operator=(Process&&) = default;

		id_type id() const { return m_nameHash; }

		void setOperation(delegate<void(time::time_span<fast_time>)>&& operation)
		{
			m_operation = operation;
		}

		void setInterval(time::time_span<fast_time> interval)
		{
			m_fixedTimeStep = interval != time::time_span<fast_time>::zero();

			m_interval = interval;
		}

		bool execute(float timeScale)
		{
			time::time_span<fast_time> deltaTime = m_clock.restart();

			if (deltaTime < 0)
				deltaTime = 0;

			if (!m_fixedTimeStep)
			{
				m_operation.invoke(deltaTime);
				return true;
			}
			else
			{
				m_timeBuffer += deltaTime;

				if (m_timeBuffer >= m_interval)
				{
					m_timeBuffer -= m_interval;

					m_operation.invoke(m_interval);
				}

				return m_timeBuffer < m_interval;
			}

			return false;
		}
	};
}