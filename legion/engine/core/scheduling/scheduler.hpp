#pragma once
#include <core/platform/platform.hpp>
#include <core/logging/logging.hpp>
#include <core/types/types.hpp>
#include <core/containers/containers.hpp>
#include <core/scheduling/processchain.hpp>
#include <core/async/async.hpp>
#include <core/common/exception.hpp>
#include <core/events/events.hpp>

#include <Optick/optick.h>

#include <memory>
#include <thread>
#include <atomic>
#include <sstream>
#include <limits>
#include <queue>

/**@file scheduler.hpp
 */

namespace legion::core::scheduling
{
    /**@brief Invalid thread id.
     */
    const std::thread::id invalid_thread_id = std::thread::id();

    /**@class Scheduler
     * @brief Major engine part that handles the creation and destruction of threads and process chains. Also takes charge of the main program loop.
     * @note Also handle synchronization.
     */
    class Scheduler
    {
    public:
        /**@class runnable
         * @brief A data structure encapsulating commands and jobs sent to any thread and it's parameters.
         */
        struct runnable
        {
            runnable() = default;
            runnable(const delegate<void(void*)>& func, void* param) : func(func), param(param) {}

            delegate<void(void*)> func;
            void* param;

            void operator()(void)
            {
                if (func != nullptr)
                    func(param);
            }
        };

    private:
        struct thread_error
        {
            std::string message;
            std::thread::id threadId;
        };

#if USE_OPTICK
        async::spinlock m_threadScopesLock;
        std::vector<std::unique_ptr<Optick::ThreadScope>> m_threadScopes;
#endif

        ProcessChain m_localChain;
        async::rw_spinlock m_processChainsLock;
        sparse_map<id_type, ProcessChain> m_processChains;
        sparse_map<id_type, std::thread::id> m_chainThreads;
        async::rw_spinlock m_errorsLock;
        std::vector<thread_error> m_errors;

        async::rw_spinlock m_exitsLock;
        std::vector<std::thread::id> m_exits;

        std::atomic_bool m_requestSync;
        async::ring_sync_lock m_syncLock;

        std::atomic<float> m_timeScale = 1.f;

        events::EventBus* m_eventBus;

        bool m_threadsShouldTerminate = false;
        bool m_threadsShouldStart = false;
        bool m_lowPower;

        static async::rw_spinlock m_threadsLock;
        static sparse_map<std::thread::id, std::unique_ptr<std::thread>> m_threads;
        static std::queue<std::thread::id> m_unreservedThreads;
        static const uint m_maxThreadCount;
        static async::rw_spinlock m_availabilityLock;
        static uint m_availableThreads;

        static async::rw_spinlock m_jobQueueLock;
        static std::queue<runnable> m_jobs;
        static sparse_map<std::thread::id, async::rw_spinlock> m_commandLocks;
        static sparse_map<std::thread::id, std::queue<Scheduler::runnable>> m_commands;

        static void threadMain(bool* exit, bool* start, bool lowPower);

    public:
        Scheduler(events::EventBus* eventBus, bool lowPower, uint minThreads);

        ~Scheduler();

        /**@brief Set global time scale.
         */
        void setTimeScale(float scale)
        {
            m_timeScale.store(scale, std::memory_order_relaxed);
        }

        /**@brief Get the global time scale.
         */
        float getTimeScale()
        {
            return m_timeScale.load(std::memory_order_relaxed);
        }

        /**@brief Run main program loop, also starts all process-chains in their own threads.
         */
        void run();

        /**@brief Create a new thread.
         * @param function Function to run on the thread.
         * @param ...args Arguments to pass to the function.
         * @return std::thread::id Id of new thread if thread was created, 0 id if there are no available threads.
         */
        template<typename Function, typename... Args >
        std::thread::id createThread(Function&& function, Args&&... args)
        {
            async::readwrite_multiguard guard(m_availabilityLock, m_threadsLock);

            if (m_availableThreads) // Check if there are available threads.
            {
                m_availableThreads--;
                std::unique_ptr<std::thread> newThread = std::make_unique<std::thread>(function, args...); // Create a new thread and run it.
                std::thread::id id = newThread->get_id();
                m_unreservedThreads.push(id);
                m_threads.insert(id, std::move(newThread));
                return id;
            }

            return std::thread::id();
        }

        template<size_type charc>
        std::thread::id getChainThreadId(const char(&name)[charc])
        {
            id_type chainId = nameHash<charc>(name);
            return m_chainThreads[chainId];
        }

        std::thread::id getChainThreadId(id_type chainId)
        {
            return m_chainThreads[chainId];
        }

        void sendCommand(std::thread::id id, delegate<void(void*)> command, void* parameter = nullptr);

        void queueJob(delegate<void(void*)> job, void* parameter);

        /**@brief Destroy a thread.
         * @warning DON'T USE UNLESS YOU KNOW WHAT YOU ARE DOING.
         */
        void destroyThread(std::thread::id id);

        /**@brief Report an intentional exit from a thread.
         */
        void reportExit(const std::thread::id& id);

        /**@brief Report an unintentional exit from a thread.
         */
        void reportExitWithError(const std::string& name, const std::thread::id& id, const legion::core::exception& exc);

        /**@brief Report an unintentional exit from a thread.
         */
        void reportExitWithError(const std::thread::id& id, const legion::core::exception& exc);

        /**@brief Report an unintentional exit from a thread.
         */
        void reportExitWithError(const std::string& name, const std::thread::id& id, const std::exception& exc);

        /**@brief Report an unintentional exit from a thread.
         */
        void reportExitWithError(const std::thread::id& id, const std::exception& exc);

        void subscribeToSync()
        {
            m_syncLock.subscribe();
        }
        
        void unsubscribeFromSync()
        {
            m_syncLock.unsubscribe();
        }

        /**@brief Request thread synchronization and wait for that synchronization moment.
         */
        void waitForProcessSync();

        /**@brief Check if a synchronization has been requested.
         */
        bool syncRequested() { return m_requestSync.load(std::memory_order_acquire); }

        /**@brief Get pointer to a certain process-chain.
         */
        template<size_type charc>
        ProcessChain* getChain(const char(&name)[charc])
        {
            id_type id = nameHash<charc>(name);
            async::readonly_guard guard(m_processChainsLock);
            if (m_processChains.contains(id))
                return &m_processChains.get(id);
            return nullptr;
        }

        /**@brief Create a new process-chain.
         */
        template<size_type charc>
        ProcessChain* addProcessChain(const char(&name)[charc])
        {
            if (!m_localChain.id())
            {
                m_localChain = ProcessChain(name, this);
                return &m_localChain;
            }

            id_type id = nameHash<charc>(name);

            std::thread::id chainThreadId;
            if (!m_unreservedThreads.empty())
            {
                chainThreadId = m_unreservedThreads.front();
                m_unreservedThreads.pop();
            }

            m_chainThreads[id] = chainThreadId;

            log::impl::thread_names[chainThreadId] = std::string(name);
#if USE_OPTICK
            sendCommand(chainThreadId, [&](void* param)
                {
                    (void)param;
                    log::info("Thread {} assigned.", std::this_thread::get_id());

                    std::lock_guard guard(m_threadScopesLock);
                    m_threadScopes.push_back(std::make_unique<Optick::ThreadScope>(legion::core::log::impl::thread_names[std::this_thread::get_id()].c_str()));
                    OPTICK_UNUSED(*m_threadScopes[m_threadScopes.size() - 1]);
                });
#else
            sendCommand(chainThreadId, [](void* param)
                {
                    (void)param;
                    log::info("Thread {} assigned.", std::this_thread::get_id());
                });
#endif
            async::readwrite_guard guard(m_processChainsLock);
            return &m_processChains.emplace(id, name, this).first.value();
        }


        /**@brief Hook a process to a certain chain.
         * @return bool True if succeeded, false if the chain doesn't exist.
         */
        template<size_type charc>
        bool hookProcess(const char(&processChainName)[charc], Process* process)
        {
            id_type chainId = nameHash<charc>(processChainName);
            async::readonly_guard guard(m_processChainsLock);
            if (m_processChains.contains(chainId))
            {
                m_processChains[chainId].addProcess(process);
                return true;
            }
            else if (m_localChain.id() == chainId)
            {
                m_localChain.addProcess(process);
                return true;
            }

            return false;
        }

        /**@brief Hook a process to a certain chain.
         * @return bool True if succeeded, false if the chain doesn't exist.
         */
        bool hookProcess(cstring chainName, Process* process);


        /**@brief Unhook a process from a certain chain.
         * @return bool True if succeeded, false if the chain doesn't exist.
         */
        template<size_type charc>
        bool unhookProcess(const char(&chainName)[charc], Process* process)
        {
            id_type chainId = nameHash<charc>(chainName);
            async::readonly_guard guard(m_processChainsLock);
            if (m_processChains.contains(chainId))
            {
                m_processChains[chainId].removeProcess(process);
                return true;
            }
            else if (m_localChain.id() == chainId)
            {
                m_localChain.removeProcess(process);
                return true;
            }

            return false;
        }

        /**@brief Unhook a process from a certain chain.
         * @return bool True if succeeded, false if the chain doesn't exist.
         */
        bool unhookProcess(cstring chainName, Process* process);

    };
}
