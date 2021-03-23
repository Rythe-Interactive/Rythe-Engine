#pragma once
#include <thread>
#include <unordered_map>

#include <Optick/optick.h>

#include <core/async/async.hpp>
#include <core/containers/pointer.hpp>
#include <core/containers/sparse_map.hpp>
#include <core/types/types.hpp>

#include <core/scheduling/process.hpp>
#include <core/scheduling/processchain.hpp>

namespace legion::core::scheduling
{
    const inline static std::thread::id invalid_thread_id = std::thread::id();

    class Scheduler
    {
    public:
        using chain_callback_type = typename ProcessChain::chain_callback_type;
        using chain_callback_delegate = typename ProcessChain::chain_callback_delegate;

    private:
#if USE_OPTICK
        static async::spinlock m_threadScopesLock;
        static std::vector<std::unique_ptr<Optick::ThreadScope>> m_threadScopes;
#endif
        static sparse_map<id_type, ProcessChain> m_processChains;
        static float m_timeScale;

        static const size_type m_maxThreadCount;
        static size_type m_availableThreads;
        static std::queue<std::thread::id> m_unreservedThreads;
        static std::unordered_map<std::thread::id, std::thread> m_threads;

        static std::unordered_map<std::thread::id, async::rw_spinlock> m_commandLocks;
        static std::unordered_map<std::thread::id, std::queue<std::unique_ptr<async::async_runnable_base>>> m_commands;

        template<typename Function, typename... Args >
        static pointer<std::thread> createThread(Function&& function, Args&&... args)
        {
            if (m_availableThreads) // Check if there are available threads.
            {
                m_availableThreads--;

                std::thread newThread{ std::forward<Function>(function), std::forward<Args>(args)... }; // Create a new thread and run it.
                std::thread::id id = newThread.get_id();
                m_unreservedThreads.push(id);
                auto [it, _] = m_threads.emplace(id, std::move(newThread));
                return { &it->second };
            }

            return { nullptr };
        }

    public:
        template<typename functor, typename... argument_types>
        static pointer<std::thread> reserveThread(functor&& function, argument_types&&... args)
        {
            return createThread(std::forward<functor>(function), std::forward<argument_types>(args)...);
        }

        static pointer<std::thread> getThread(std::thread::id id);

        template<typename functor>
        static auto sendCommand(std::thread::id id, functor&& function, size_type taskSize = 1)
        {
            auto* command = new async::async_runnable(function, taskSize);

            {
                async::readwrite_guard guard(m_commandLocks[id]);
                m_commands[id].push(std::unique_ptr<async::async_runnable_base>(command));
            }

            return command->getOperation(
                [](std::thread::id l_id, auto l_func, size_type l_taskSize = 1) { return sendCommand(l_id, l_func, l_taskSize); }
            );
        }

        static int run(bool low_power = false, uint minThreads = 0);

        static void exit(int exitCode);

        /**@brief Get pointer to a certain process-chain.
         */
        template<size_type charc>
        static pointer<ProcessChain> getChain(const char(&name)[charc])
        {
            id_type id = nameHash<charc>(name);
            if (m_processChains.contains(id))
                return { &m_processChains.at(id) };
            return { nullptr };
        }

        /**@brief Get pointer to a certain process-chain.
         */
        static pointer<ProcessChain> getChain(id_type id)
        {
            if (m_processChains.contains(id))
                return { &m_processChains.at(id) };
            return { nullptr };
        }

        /**@brief Get pointer to a certain process-chain.
         */
        static pointer<ProcessChain> getChain(cstring name)
        {
            id_type id = nameHash(name);
            if (m_processChains.contains(id))
                return { &m_processChains.at(id) };
            return { nullptr };
        }

        static void subscribeToChainStart(id_type chainId, const chain_callback_delegate& callback);
        static void subscribeToChainStart(cstring chainName, const chain_callback_delegate& callback);

        static void unsubscribeFromChainStart(id_type chainId, const chain_callback_delegate& callback);
        static void unsubscribeFromChainStart(cstring chainName, const chain_callback_delegate& callback);

        static void subscribeToChainEnd(id_type chainId, const chain_callback_delegate& callback);
        static void subscribeToChainEnd(cstring chainName, const chain_callback_delegate& callback);

        static void unsubscribeFromChainEnd(id_type chainId, const chain_callback_delegate& callback);
        static void unsubscribeFromChainEnd(cstring chainName, const chain_callback_delegate& callback);

        /**@brief Hook a process to a certain chain.
         * @return bool True if succeeded, false if the chain doesn't exist.
         */
        template<size_type charc>
        static bool hookProcess(const char(&processChainName)[charc], Process& process)
        {
            id_type chainId = nameHash<charc>(processChainName);

            if (m_processChains.contains(chainId))
            {
                m_processChains[chainId].addProcess(process);
                return true;
            }

            return false;
        }

        /**@brief Hook a process to a certain chain.
         * @return bool True if succeeded, false if the chain doesn't exist.
         */
        static bool hookProcess(cstring chainName, Process& process)
        {
            id_type chainId = nameHash(chainName);

            if (m_processChains.contains(chainId))
            {
                m_processChains[chainId].addProcess(process);
                return true;
            }

            return false;
        }

        /**@brief Unhook a process from a certain chain.
         * @return bool True if succeeded, false if the chain doesn't exist.
         */
        template<size_type charc>
        static bool unhookProcess(const char(&chainName)[charc], Process& process)
        {
            id_type chainId = nameHash<charc>(chainName);

            if (m_processChains.contains(chainId))
                return m_processChains[chainId].removeProcess(process);

            return false;
        }

        /**@brief Unhook a process from a certain chain.
         * @return bool True if succeeded, false if the chain doesn't exist.
         */
        static bool unhookProcess(cstring chainName, Process& process)
        {
            OPTICK_EVENT();
            id_type chainId = nameHash(chainName);

            if (m_processChains.contains(chainId))
                return m_processChains[chainId].removeProcess(process);

            return false;
        }
    };
}
