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
#include <core/scheduling/clock.hpp>

namespace legion::core::scheduling
{
    const inline static std::thread::id invalid_thread_id = std::thread::id();

    class Scheduler
    {
    public:
        using chain_callback_type = typename ProcessChain::chain_callback_type;
        using chain_callback_delegate = typename ProcessChain::chain_callback_delegate;
        using frame_callback_type = chain_callback_type;
        using frame_callback_delegate = chain_callback_delegate;

    private:
        template<typename resource>
        using per_thread_map = std::unordered_map<std::thread::id, resource>;

        static sparse_map<id_type, ProcessChain> m_processChains;

        static multicast_delegate<frame_callback_type> m_onFrameStart;
        static multicast_delegate<frame_callback_type> m_onFrameEnd;

        static const size_type m_maxThreadCount;
        static size_type m_availableThreads;

        static per_thread_map<std::thread> m_threads;

        static per_thread_map<async::rw_lock_pair<async::runnables_queue>> m_commands;
        static async::rw_lock_pair<async::job_queue> m_jobs;
        static size_type m_jobPoolSize;

        static std::atomic<bool> m_exit;
        static std::atomic<bool> m_start;
        static int m_exitCode;


        static void threadMain(bool lowPower, std::string name);

        template<typename Function, typename... Args >
        L_NODISCARD static pointer<std::thread> createThread(Function&& function, Args&&... args);

        static void tryCompleteJobPool();

        static void doTick(Clock::span_type deltaTime);

    public:
        static std::atomic<float> m_pollTime;

        template<typename functor, typename... argument_types>
        L_NODISCARD static pointer<std::thread> reserveThread(functor&& function, argument_types&&... args);

        L_NODISCARD static pointer<std::thread> getThread(std::thread::id id);

        template<typename functor>
        static auto sendCommand(std::thread::id id, functor&& function, float taskSize = 1.f);

        static size_type jobPoolSize() noexcept;

        template<typename Func>
        static auto queueJobs(size_type count, Func&& func);

        static int run(bool lowPower = false, size_type minThreads = 0);

        static void exit(int exitCode);

        static bool isExiting();

        /**@brief Create a new process-chain.
         */
        template<size_type charc>
        static pointer<ProcessChain> createProcessChain(const char(&name)[charc]);

        /**@brief Create a new process-chain.
         */
        static pointer<ProcessChain> createProcessChain(cstring name);

        /**@brief Get pointer to a certain process-chain.
         */
        template<size_type charc>
        L_NODISCARD static pointer<ProcessChain> getChain(const char(&name)[charc]);

        /**@brief Get pointer to a certain process-chain.
         */
        L_NODISCARD static pointer<ProcessChain> getChain(id_type id);

        /**@brief Get pointer to a certain process-chain.
         */
        L_NODISCARD static pointer<ProcessChain> getChain(cstring name);

        static void subscribeToFrameStart(const frame_callback_delegate& callback);
        static void unsubscribeFromFrameStart(const frame_callback_delegate& callback);

        static void subscribeToFrameEnd(const frame_callback_delegate& callback);
        static void unsubscribeFromFrameEnd(const frame_callback_delegate& callback);

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
        template<size_type charc>
        static bool hookProcess(const char(&processChainName)[charc], pointer<Process> process);

        /**@brief Hook a process to a certain chain.
         * @return bool True if succeeded, false if the chain doesn't exist.
         */
        static bool hookProcess(cstring chainName, Process& process);

        /**@brief Hook a process to a certain chain.
         * @return bool True if succeeded, false if the chain doesn't exist.
         */
        static bool hookProcess(cstring chainName, pointer<Process> process);

        /**@brief Unhook a process from a certain chain.
         * @return bool True if succeeded, false if the chain doesn't exist.
         */
        template<size_type charc>
        static bool unhookProcess(const char(&chainName)[charc], Process& process);

        /**@brief Unhook a process from a certain chain.
         * @return bool True if succeeded, false if the chain doesn't exist.
         */
        template<size_type charc>
        static bool unhookProcess(const char(&chainName)[charc], pointer<Process> process);

        /**@brief Unhook a process from a certain chain.
         * @return bool True if succeeded, false if the chain doesn't exist.
         */
        static bool unhookProcess(cstring chainName, Process& process);

        /**@brief Unhook a process from a certain chain.
         * @return bool True if succeeded, false if the chain doesn't exist.
         */
        static bool unhookProcess(cstring chainName, pointer<Process> process);

        /**@brief Unhook a process from a certain chain.
         * @return bool True if succeeded, false if the chain doesn't exist.
         */
        static bool unhookProcess(id_type chainId, pointer<Process> process);
    };
}

#include <core/scheduling/scheduler.inl>
