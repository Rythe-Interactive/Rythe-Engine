#include <core/scheduling/scheduler.hpp>
#pragma once

namespace legion::core::scheduling
{
    template<typename Function, typename ...Args>
    inline L_ALWAYS_INLINE pointer<std::thread> scheduling::Scheduler::createThread(Function&& function, Args && ...args)
    {
        if (m_availableThreads) // Check if there are available threads.
        {
            m_availableThreads--;

            std::thread newThread{ std::forward<Function>(function), std::forward<Args>(args)... }; // Create a new thread and run it.
            std::thread::id id = newThread.get_id();

            auto [it, _] = m_threads.emplace(id, std::move(newThread));
            return { &it->second };
        }

        return { nullptr };
    }

    template<typename functor, typename ...argument_types>
    inline L_ALWAYS_INLINE pointer<std::thread> Scheduler::reserveThread(functor&& function, argument_types && ...args)
    {
        return createThread(std::forward<functor>(function), std::forward<argument_types>(args)...);
    }

    template<typename functor>
    inline L_ALWAYS_INLINE auto Scheduler::sendCommand(std::thread::id id, functor&& function, float taskSize)
    {
        auto* command = new async::async_runnable(function, taskSize);

        {
            auto& [lock, commandQueue] = m_commands[id];
            async::readwrite_guard guard(lock);
            commandQueue.push(std::unique_ptr<async::async_runnable_base>(command));
        }

        return command->getOperation(
            [](std::thread::id l_id, auto l_func, float l_taskSize = 1) { return sendCommand(l_id, l_func, l_taskSize); }
        );
    }

    template<typename Func>
    inline L_ALWAYS_INLINE auto Scheduler::queueJobs(size_type count, Func&& func)
    {
        auto repeater = [](size_type l_count, auto l_func) { return queueJobs(l_count, l_func); };

        if (!count)
            return async::job_operation(std::shared_ptr<async::async_progress>(nullptr), std::shared_ptr<async::job_pool>(nullptr), repeater, tryCompleteJobPool);

        OPTICK_EVENT("legion::core::scheduling::Scheduler::queueJobs<T>");
        std::shared_ptr<async::job_pool> jobPool = std::make_shared<async::job_pool>(count, func);

        auto& [lock, jobQueue] = m_jobs;
        async::readwrite_guard guard(lock);
        jobQueue.push(jobPool);

        return async::job_operation(jobPool->get_progress(), jobPool, repeater, tryCompleteJobPool);
    }

    template<size_type charc>
    inline L_ALWAYS_INLINE pointer<ProcessChain> Scheduler::createProcessChain(const char(&name)[charc])
    {
        id_type id = nameHash<charc>(name);
        return { &m_processChains.emplace(id, name, id).first.value() };
    }

    template<size_type charc>
    inline L_ALWAYS_INLINE pointer<ProcessChain> Scheduler::getChain(const char(&name)[charc])
    {
        id_type id = nameHash<charc>(name);
        if (m_processChains.contains(id))
            return { &m_processChains.at(id) };
        return { nullptr };
    }

    template<size_type charc>
    inline L_ALWAYS_INLINE bool Scheduler::hookProcess(const char(&processChainName)[charc], Process& process)
    {
        id_type chainId = nameHash<charc>(processChainName);

        if (m_processChains.contains(chainId))
        {
            m_processChains[chainId].addProcess(process);
            return true;
        }

        return false;
    }

    template<size_type charc>
    inline L_ALWAYS_INLINE bool Scheduler::hookProcess(const char(&processChainName)[charc], pointer<Process> process)
    {
        id_type chainId = nameHash<charc>(processChainName);

        if (m_processChains.contains(chainId))
        {
            m_processChains[chainId].addProcess(process);
            return true;
        }

        return false;
    }

    template<size_type charc>
    inline L_ALWAYS_INLINE bool Scheduler::unhookProcess(const char(&chainName)[charc], Process& process)
    {
        id_type chainId = nameHash<charc>(chainName);

        if (m_processChains.contains(chainId))
            return m_processChains[chainId].removeProcess(process);

        return false;
    }

    template<size_type charc>
    inline L_ALWAYS_INLINE bool Scheduler::unhookProcess(const char(&chainName)[charc], pointer<Process> process)
    {
        id_type chainId = nameHash<charc>(chainName);

        if (m_processChains.contains(chainId))
            return m_processChains[chainId].removeProcess(process);

        return false;
    }

}
