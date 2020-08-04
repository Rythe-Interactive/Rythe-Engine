#include <core/scheduling/process.hpp>

namespace args::core::scheduling
{
	inline void Process::setOperation(delegate<void(time::time_span<fast_time>)>&& operation)
	{
		m_operation = operation;
	}

	inline void Process::setInterval(time::time_span<fast_time> interval)
	{
		m_fixedTimeStep = interval != time::time_span<fast_time>::zero();

		m_interval = interval;
	}

	bool Process::execute(float timeScale)
	{
		time::time_span<fast_time> deltaTime = m_clock.restart();

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
}