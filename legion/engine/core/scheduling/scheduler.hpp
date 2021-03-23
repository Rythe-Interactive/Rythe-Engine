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
        static sparse_map<id_type, ProcessChain> m_processChains;

        static const size_type m_maxThreadCount;
        static size_type m_availableThreads;
        static std::queue<std::thread::id> m_unreservedThreads;
        static std::unordered_map<std::thread::id, std::thread> m_threads;

        static std::unordered_map<std::thread::id, std::pair<async::rw_spinlock, std::queue<std::unique_ptr<async::async_runnable_base>>>> m_commands;

        static async::rw_spinlock m_jobQueueLock;
        static std::queue<std::shared_ptr<async::job_pool>> m_jobs;

        static bool m_exit;
        static int m_exitCode;

        template<typename Function, typename... Args >
        static pointer<std::thread> createThread(Function&& function, Args&&... args);

    public:
        template<typename functor, typename... argument_types>
        static pointer<std::thread> reserveThread(functor&& function, argument_types&&... args);

        static pointer<std::thread> getThread(std::thread::id id);

        template<typename functor>
        static auto sendCommand(std::thread::id id, functor&& function, size_type taskSize = 1);

        static int run(bool lowPower = false, uint minThreads = 0);

        static void exit(int exitCode);

        /**@brief Get pointer to a certain process-chain.
         */
        template<size_type charc>
        static pointer<ProcessChain> getChain(const char(&name)[charc]);

        /**@brief Get pointer to a certain process-chain.
         */
        static pointer<ProcessChain> getChain(id_type id);

        /**@brief Get pointer to a certain process-chain.
         */
        static pointer<ProcessChain> getChain(cstring name);

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
        static bool hookProcess(const char(&processChainName)[charc], Process& process);

        /**@brief Hook a process to a certain chain.
         * @return bool True if succeeded, false if the chain doesn't exist.
         */
        static bool hookProcess(cstring chainName, Process& process);

        /**@brief Unhook a process from a certain chain.
         * @return bool True if succeeded, false if the chain doesn't exist.
         */
        template<size_type charc>
        static bool unhookProcess(const char(&chainName)[charc], Process& process);

        /**@brief Unhook a process from a certain chain.
         * @return bool True if succeeded, false if the chain doesn't exist.
         */
        static bool unhookProcess(cstring chainName, Process& process);
    };
}

#include <core/scheduling/scheduler.inl>
