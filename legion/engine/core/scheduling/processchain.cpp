#include <core/scheduling/processchain.hpp>
#include <core/scheduling/scheduler.hpp>
#include <core/scheduling/clock.hpp>

namespace legion::core::scheduling
{
    id_type ProcessChain::id()
    {
        return m_nameHash;
    }

    void ProcessChain::subscribeToChainStart(const chain_callback_delegate& callback)
    {
        m_onChainStart.push_back(callback);
    }

    void ProcessChain::unsubscribeFromChainStart(const chain_callback_delegate& callback)
    {
        m_onChainStart.erase(callback);
    }

    void ProcessChain::subscribeToChainEnd(const chain_callback_delegate& callback)
    {
        m_onChainEnd.push_back(callback);
    }

    void ProcessChain::unsubscribeFromChainEnd(const chain_callback_delegate& callback)
    {
        m_onChainEnd.erase(callback);
    }

    void ProcessChain::addProcess(Process& process)
    {
        m_processes.insert(process.id(), pointer<Process>{ &process });
    }

    bool ProcessChain::removeProcess(Process& process)
    {
        return m_processes.erase(process.id());
    }

    void ProcessChain::runInCurrentThread(time::span deltaTime)
    {
        m_onChainStart(deltaTime, time::span(Clock::elapsedSinceTickStart()));

        for (auto [_, ptr] : m_processes)
            ptr->execute(deltaTime);

        m_onChainEnd(deltaTime, time::span(Clock::elapsedSinceTickStart()));
    }

}
