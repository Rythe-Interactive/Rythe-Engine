#include <core/scheduling/scheduler.hpp>
#include <core/scheduling/clock.hpp>

namespace legion::core::scheduling
{
    void Scheduler::threadMain(bool lowPower)
    {
        while (!m_exit)
        {
            {
                auto& [lock, commandQueue] = m_commands.at(std::this_thread::get_id());

                async::readonly_guard guard(lock);

                commandQueue.front()->execute();
                commandQueue.pop();
            }

            {
                auto& [lock, jobQueue] = m_jobs;
                async::readonly_guard guard(lock);

                auto jobPoolPtr = jobQueue.front();
                if (jobPoolPtr->is_done())
                {
                    async::readwrite_guard wguard(lock);
                    if (jobPoolPtr->is_done())
                        jobQueue.pop();
                }
                else
                    jobPoolPtr->complete_job();
            }

            if (lowPower)
                std::this_thread::yield();
            else
                L_PAUSE_INSTRUCTION();
        }
    }

    pointer<std::thread> Scheduler::getThread(std::thread::id id)
    {
        return { &m_threads.at(id) };
    }

    int Scheduler::run(bool lowPower, uint minThreads)
    {
        bool m_lowPower = lowPower;

        if (std::thread::hardware_concurrency() < minThreads)
            m_lowPower = true;

        if (m_availableThreads < minThreads)
            m_availableThreads = minThreads;

        pointer<std::thread> ptr;
        while ((ptr = createThread(Scheduler::threadMain, m_lowPower)) != nullptr)
        {
            m_commands.try_emplace(ptr->get_id());
        }

        while (!m_exit)
        {
            time::span dt{ Clock::lastTickDuration() };
            for (auto [_, chain] : m_processChains)
                chain.runInCurrentThread(dt);

            if (m_lowPower)
                std::this_thread::yield();
            else
                L_PAUSE_INSTRUCTION();
        }

        return m_exitCode;
    }

    void Scheduler::exit(int exitCode)
    {
        m_exitCode = exitCode;
        m_exit = true;
    }

    pointer<ProcessChain> Scheduler::getChain(id_type id)
    {
        if (m_processChains.contains(id))
            return { &m_processChains.at(id) };
        return { nullptr };
    }

    pointer<ProcessChain> Scheduler::getChain(cstring name)
    {
        id_type id = nameHash(name);
        if (m_processChains.contains(id))
            return { &m_processChains.at(id) };
        return { nullptr };
    }

    void Scheduler::subscribeToChainStart(id_type chainId, const chain_callback_delegate& callback)
    {
    }

    void Scheduler::subscribeToChainStart(cstring chainName, const chain_callback_delegate& callback)
    {
    }

    void Scheduler::unsubscribeFromChainStart(id_type chainId, const chain_callback_delegate& callback)
    {
    }

    void Scheduler::unsubscribeFromChainStart(cstring chainName, const chain_callback_delegate& callback)
    {
    }

    void Scheduler::subscribeToChainEnd(id_type chainId, const chain_callback_delegate& callback)
    {
    }

    void Scheduler::subscribeToChainEnd(cstring chainName, const chain_callback_delegate& callback)
    {
    }

    void Scheduler::unsubscribeFromChainEnd(id_type chainId, const chain_callback_delegate& callback)
    {
    }

    void Scheduler::unsubscribeFromChainEnd(cstring chainName, const chain_callback_delegate& callback)
    {
    }

    bool Scheduler::hookProcess(cstring chainName, Process& process)
    {
        id_type chainId = nameHash(chainName);

        if (m_processChains.contains(chainId))
        {
            m_processChains[chainId].addProcess(process);
            return true;
        }

        return false;
    }

    bool Scheduler::unhookProcess(cstring chainName, Process& process)
    {
        OPTICK_EVENT();
        id_type chainId = nameHash(chainName);

        if (m_processChains.contains(chainId))
            return m_processChains[chainId].removeProcess(process);

        return false;
    }

}
