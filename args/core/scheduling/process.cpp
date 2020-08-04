#include <core/scheduling/process.hpp>

namespace args::core::scheduling
{
	void Process::setOperation(delegate<void(fast_time)>&& operation)
	{
		m_operation = operation;
	}

	void Process::setInterval(fast_seconds interval)
	{
	}
	
	bool Process::execute(fast_seconds deltaTime)
	{
		return false;
	}
}