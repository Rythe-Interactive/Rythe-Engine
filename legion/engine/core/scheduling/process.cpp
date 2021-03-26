#include <core/scheduling/process.hpp>

namespace legion::core::scheduling
{
    Process::Process(const std::string& name, id_type nameHash, time::span interval) : m_name(name), m_nameHash(nameHash)
    {
        setInterval(interval);
    }

    id_type Process::id() const noexcept
    {
        return m_nameHash;
    }

    bool Process::inUse() const noexcept
    {
        return m_hooks.size();
    }

    void Process::setOperation(delegate<void(time::time_span<fast_time>)>&& operation) noexcept
    {
        m_operation = operation;
    }

    void Process::setInterval(time::span interval) noexcept
    {
        m_fixedTimeStep = interval != time::span::zero();
        m_interval = interval;
    }

    std::unordered_set<id_type>& Process::hooks() noexcept
    {
        return m_hooks;
    }

    void Process::execute(time::span deltaTime)
    {
        OPTICK_EVENT("Execute process");
        OPTICK_TAG("Process", m_name.c_str());

        if (!m_fixedTimeStep)
        {
            m_operation(deltaTime);
        }
        else
        {
            m_timebuffer += deltaTime;
            if (m_timebuffer >= m_interval)
            {
                m_operation(m_timebuffer);
                m_timebuffer -= m_interval;
            }
        }
    }
}
