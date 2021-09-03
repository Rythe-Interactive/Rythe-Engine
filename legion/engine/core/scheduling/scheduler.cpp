#include <core/scheduling/scheduler.hpp>
#include <core/events/events.hpp>

namespace legion::core::scheduling
{
    constexpr size_type reserved_threads = 1; // this, OS

    sparse_map<id_type, ProcessChain> Scheduler::m_processChains;

    multicast_delegate<Scheduler::frame_callback_type> Scheduler::m_onFrameStart;
    multicast_delegate<Scheduler::frame_callback_type> Scheduler::m_onFrameEnd;

    const size_type Scheduler::m_maxThreadCount = reserved_threads >= std::thread::hardware_concurrency() ? 0 : std::thread::hardware_concurrency() - reserved_threads;
    size_type Scheduler::m_availableThreads = m_maxThreadCount;

    Scheduler::per_thread_map<std::thread> Scheduler::m_threads;

    async::rw_lock_pair<async::job_queue> Scheduler::m_jobs;
    size_type Scheduler::m_jobPoolSize = 0;

    std::atomic<bool> Scheduler::m_exit = { false };
    std::atomic<bool> Scheduler::m_exitFromEvent = { false };
    std::atomic<bool> Scheduler::m_start = { false };
    int Scheduler::m_exitCode = 0;

    std::atomic<float> Scheduler::m_pollTime = { 0.1f };

    void Scheduler::threadMain(bool lowPower, std::string name)
    {
        log::info("Thread {} assigned.", std::this_thread::get_id());
        async::set_thread_name(name.c_str());

        OPTICK_THREAD(name.c_str());

        while (!m_start.load(std::memory_order_relaxed))
            std::this_thread::yield();

        time::timer clock;
        time::span timeBuffer;
        time::span sleepTime;

        while (!m_exit.load(std::memory_order_relaxed))
        {
            bool noWork = true;

            std::shared_ptr<async::job_pool> jobPoolPtr = nullptr;

            {
                auto& [lock, jobQueue] = m_jobs;
                async::readonly_guard guard(lock);
                if (!jobQueue.empty())
                {
                    noWork = false;

                    jobPoolPtr = jobQueue.front();
                    if (!jobPoolPtr->is_done())
                    {
                        if (jobPoolPtr->prime_job()) // Returns true when this is the last job.
                        {
                            async::readwrite_guard wguard(lock);
                            if (!jobQueue.empty())
                            {
                                if (jobQueue.front() == jobPoolPtr)
                                    jobQueue.pop_front();
                                else
                                    jobQueue.remove(jobPoolPtr);
                            }
                        }
                    }
                    else
                    {
                        async::readwrite_guard wguard(lock);
                        if (!jobQueue.empty())
                        {
                            if (jobQueue.front() == jobPoolPtr)
                                jobQueue.pop_front();
                            else
                                jobQueue.remove(jobPoolPtr);
                        }
                        jobPoolPtr = nullptr;
                    }
                }
            }
            if (jobPoolPtr)
                jobPoolPtr->complete_job();

            if (noWork)
            {
                timeBuffer += clock.restart();

                if (lowPower || LEGION_CONFIGURATION == LEGION_DEBUG_VALUE)
                {
                    OPTICK_EVENT("Sleep");
                    std::this_thread::sleep_for(std::chrono::microseconds(1));
                }
                else if (timeBuffer >= sleepTime * m_pollTime.load(std::memory_order_relaxed))
                {
                    OPTICK_EVENT("Sleep");
                    timeBuffer -= sleepTime;

                    time::timer sleepTimer;
                    sleepTimer.start();
                    std::this_thread::sleep_for(std::chrono::nanoseconds(1));
                    sleepTime = sleepTimer.elapsed_time();
                }
                else
                    std::this_thread::yield();
            }
            else
            {
                timeBuffer = 0.f;
                clock.start();
            }
        }
    }

    void Scheduler::tryCompleteJobPool()
    {
        auto& [lock, jobQueue] = m_jobs;
        async::readwrite_guard wguard(lock);
        if (!jobQueue.empty() && jobQueue.front()->is_done())
        {
            jobQueue.pop_front();
        }
    }

    void Scheduler::doTick(Clock::span_type deltaTime)
    {
        OPTICK_FRAME("Main thread");

        time::span dt{ deltaTime };
        m_onFrameStart(dt, time::span(Clock::elapsedSinceTickStart()));

        for (auto [_, chain] : m_processChains)
            chain.runInCurrentThread(dt);

        m_onFrameEnd(dt, time::span(Clock::elapsedSinceTickStart()));
    }

    pointer<std::thread> Scheduler::getThread(std::thread::id id)
    {
        return { &m_threads.at(id) };
    }

    void Scheduler::init()
    {
        events::EventBus::bindToEvent<events::exit>([](events::exit& evnt)
            {
                scheduling::Scheduler::m_exitFromEvent.store(true, std::memory_order_release);
                scheduling::Scheduler::exit(evnt.exitcode);
            });
    }

    int Scheduler::run(bool lowPower, size_type minThreads)
    {
        bool m_lowPower = lowPower;

        size_type m_minThreads = minThreads < 1 ? 1 : minThreads;

        if (m_maxThreadCount < m_minThreads)
            m_lowPower = true;

        if (m_availableThreads < m_minThreads)
            m_availableThreads = m_minThreads;

        pointer<std::thread> ptr;
        std::string name = "Worker ";

        {
            async::readwrite_guard guard(log::impl::threadNamesLock);
            while ((ptr = createThread(Scheduler::threadMain, m_lowPower, name + std::to_string(m_jobPoolSize))) != nullptr)
                log::impl::threadNames[ptr->get_id()] = name + std::to_string(m_jobPoolSize++);

            log::impl::threadNames[std::this_thread::get_id()] = "Main thread";
        }

        m_start.store(true, std::memory_order_release);

        Clock::subscribeToTick(doTick);

        while (!m_exit.load(std::memory_order_relaxed))
        {
            Clock::update();

            float deltaTime = static_cast<float>(Clock::lastTickDuration());
            static size_type framecount = 0;
            static float totalTime = 0;
            static uint bestAvg = 0;
            static float bestPollTime = 0.1f;

            totalTime += deltaTime;
            framecount++;

            if (totalTime > 5.f)
            {
                uint avg = math::uround(1.f / (totalTime / static_cast<float>(framecount)));
                totalTime = 0.f;
                framecount = 0;
                float pollTime = m_pollTime.load(std::memory_order_relaxed);

                log::debug("avg: {} poll: {:.3f}\tbAvg: {} bPoll: {:.3f}", avg, pollTime, bestAvg, bestPollTime);

                if (avg > bestAvg)
                {
                    bestPollTime = pollTime;
                    bestAvg = avg;
                    pollTime += (math::linearRand<int8>(0, 1) ? 0.05f : -0.05f);
                    pollTime = math::mod(pollTime + 1.f, 1.f);
                }
                else if (math::close_enough(bestPollTime, pollTime))
                {
                    if (avg < static_cast<uint>(static_cast<float>(bestAvg) * 0.9f))
                        bestAvg = 0;

                    pollTime += (math::linearRand<int8>(0, 1) ? 0.05f : -0.05f);
                    pollTime = math::mod(pollTime + 1.f, 1.f);
                }
                else
                {
                    pollTime = bestPollTime;
                }

                m_pollTime.store(pollTime, std::memory_order_relaxed);
            }

            if (m_lowPower)
                std::this_thread::yield();
            else
                L_PAUSE_INSTRUCTION();
        }

        for (auto& [_, thread] : m_threads)
            if (thread.joinable())
                thread.join();

        return m_exitCode;
    }

    void Scheduler::exit(int exitCode)
    {
        if (!m_exitFromEvent.load(std::memory_order_relaxed))
        {
            events::EventBus::raiseEvent<events::exit>(exitCode);
            return;
        }

        if (m_exit.load(std::memory_order_relaxed))
        {
            log::warn("Engine was already exiting, triggered additional exit event with code {}", exitCode);
            return;
        }

        log::undecoratedInfo("=========================\n"
            "| Shutting down engine. |\n"
            "=========================");

        m_exitCode = exitCode;
        m_exit.store(true, std::memory_order_release);
    }

    bool Scheduler::isExiting()
    {
        return m_exit.load(std::memory_order_relaxed);
    }

    size_type Scheduler::jobPoolSize() noexcept
    {
        return m_jobPoolSize;
    }

    pointer<ProcessChain> Scheduler::createProcessChain(cstring name)
    {
        id_type id = nameHash(name);
        return { &m_processChains.emplace(id, name, id).first.value() };
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
        m_processChains.at(chainId).subscribeToChainStart(callback);
    }

    void Scheduler::subscribeToChainStart(cstring chainName, const chain_callback_delegate& callback)
    {
        id_type chainId = nameHash(chainName);
        m_processChains.at(chainId).subscribeToChainStart(callback);
    }

    void Scheduler::unsubscribeFromChainStart(id_type chainId, const chain_callback_delegate& callback)
    {
        m_processChains.at(chainId).unsubscribeFromChainStart(callback);
    }

    void Scheduler::unsubscribeFromChainStart(cstring chainName, const chain_callback_delegate& callback)
    {
        id_type chainId = nameHash(chainName);
        m_processChains.at(chainId).unsubscribeFromChainStart(callback);
    }

    void Scheduler::subscribeToChainEnd(id_type chainId, const chain_callback_delegate& callback)
    {
        m_processChains.at(chainId).subscribeToChainEnd(callback);
    }

    void Scheduler::subscribeToChainEnd(cstring chainName, const chain_callback_delegate& callback)
    {
        id_type chainId = nameHash(chainName);
        m_processChains.at(chainId).subscribeToChainEnd(callback);
    }

    void Scheduler::subscribeToFrameStart(const frame_callback_delegate& callback)
    {
        m_onFrameStart.push_back(callback);
    }

    void Scheduler::unsubscribeFromFrameStart(const frame_callback_delegate& callback)
    {
        m_onFrameStart.erase(callback);
    }

    void Scheduler::subscribeToFrameEnd(const frame_callback_delegate& callback)
    {
        m_onFrameEnd.push_back(callback);
    }

    void Scheduler::unsubscribeFromFrameEnd(const frame_callback_delegate& callback)
    {
        m_onFrameEnd.erase(callback);
    }

    void Scheduler::unsubscribeFromChainEnd(id_type chainId, const chain_callback_delegate& callback)
    {
        m_processChains.at(chainId).unsubscribeFromChainEnd(callback);
    }

    void Scheduler::unsubscribeFromChainEnd(cstring chainName, const chain_callback_delegate& callback)
    {
        id_type chainId = nameHash(chainName);
        m_processChains.at(chainId).unsubscribeFromChainEnd(callback);
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

    bool Scheduler::hookProcess(cstring chainName, pointer<Process> process)
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
        id_type chainId = nameHash(chainName);

        if (m_processChains.contains(chainId))
            return m_processChains[chainId].removeProcess(process);

        return false;
    }

    bool Scheduler::unhookProcess(cstring chainName, pointer<Process> process)
    {
        id_type chainId = nameHash(chainName);

        if (m_processChains.contains(chainId))
            return m_processChains[chainId].removeProcess(process);

        return false;
    }

    bool Scheduler::unhookProcess(id_type chainId, pointer<Process> process)
    {
        if (m_processChains.contains(chainId))
            return m_processChains[chainId].removeProcess(process);

        return false;
    }

}
