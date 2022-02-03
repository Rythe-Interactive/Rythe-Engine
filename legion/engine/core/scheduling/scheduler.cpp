#include <core/scheduling/scheduler.hpp>
#include <core/events/events.hpp>
#include <core/engine/engine.hpp>

namespace legion::core::scheduling
{
    thread_local Engine* Scheduler::m_currentEngineInstance;

    multicast_delegate<Scheduler::thread_callback_type>& Scheduler::getOnThreadCreate()
    {
        static multicast_delegate<thread_callback_type> m_onThreadCreate;
        return m_onThreadCreate;
    }

    void Scheduler::onInit()
    {
        reportDependency<events::EventBus>();
        reportDependency<Clock>();

        events::EventBus::bindToEvent<events::exit>([](events::exit& evnt)
            {
                instance.m_exitFromEvent.store(true, std::memory_order_release);
                Scheduler::exit(evnt.exitcode);
            });

        create();
    }

    void Scheduler::onShutdown()
    {
        if (!isExiting())
            exit(0);
    }

    void Scheduler::threadMain(Engine& engine, bool lowPower, std::string name)
    {
        hookThreadToEngine(engine);
        getOnThreadCreate()();

        async::set_thread_name(name.c_str());
        log::info("Thread {} assigned.", std::this_thread::get_id());

        OPTICK_THREAD(name.c_str());

        while (!instance.m_start.load(std::memory_order_relaxed))
            std::this_thread::yield();

        log::info("Starting thread.");

        time::timer clock;
        time::span timeBuffer;
        time::span sleepTime;

        while (!instance.m_exit.load(std::memory_order_relaxed))
        {
            bool noWork = true;

            std::shared_ptr<async::job_pool> jobPoolPtr = nullptr;

            {
                auto& [lock, jobQueue] = instance.m_jobs;
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
                else if (timeBuffer >= sleepTime * instance.m_pollTime.load(std::memory_order_relaxed))
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

        log::info("Shutting down thread.");
    }

    void Scheduler::tryCompleteJobPool()
    {
        auto& [lock, jobQueue] = instance.m_jobs;
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
        instance.m_onFrameStart(dt, time::span(Clock::elapsedSinceTickStart()));

        for (auto [_, chain] : instance.m_processChains)
            chain.runInCurrentThread(dt);

        instance.m_onFrameEnd(dt, time::span(Clock::elapsedSinceTickStart()));
    }

    void Scheduler::hookThreadToEngine(Engine& context)
    {
        m_currentEngineInstance = &context;
        this_engine::m_ptr = &context;
    }

    pointer<Engine> Scheduler::currentEngineInstance()
    {
        return { m_currentEngineInstance };
    }

    pointer<std::thread> Scheduler::getThread(std::thread::id id)
    {
        return { &instance.m_threads.at(id) };
    }

    int Scheduler::run(Engine& engine, bool lowPower, size_type minThreads)
    {
        hookThreadToEngine(engine);
        getOnThreadCreate()();

        bool m_lowPower = lowPower;

        size_type m_minThreads = minThreads < 1 ? 1 : minThreads;

        if (instance.m_maxThreadCount < m_minThreads)
            m_lowPower = true;

        if (instance.m_availableThreads < m_minThreads)
            instance.m_availableThreads = m_minThreads;

        pointer<std::thread> ptr;
        std::string name = "Worker ";

        {
            auto& logData = log::impl::get();
            async::readwrite_guard guard(logData.threadNamesLock);
            while ((ptr = createThread(Scheduler::threadMain, std::ref(engine), m_lowPower, name + std::to_string(instance.m_jobPoolSize))) != nullptr)
                logData.threadNames[ptr->get_id()] = name + std::to_string(instance.m_jobPoolSize++);

            logData.threadNames[std::this_thread::get_id()] = "Main thread: " + std::to_string(this_engine::id().value());
        }

        instance.m_start.store(true, std::memory_order_release);

        Clock::subscribeToTick(doTick);

        while (!instance.m_exit.load(std::memory_order_relaxed))
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
                float pollTime = instance.m_pollTime.load(std::memory_order_relaxed);

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

                instance.m_pollTime.store(pollTime, std::memory_order_relaxed);
            }

            if (m_lowPower)
                std::this_thread::yield();
            else
                L_PAUSE_INSTRUCTION();
        }

        m_currentEngineInstance->shutdownModules();

        for (auto& [_, thread] : instance.m_threads)
            if (thread.joinable())
                thread.join();

        return instance.m_exitCode;
    }

    void Scheduler::exit(int exitCode)
    {
        if (!instance.m_exitFromEvent.load(std::memory_order_relaxed))
        {
            events::EventBus::raiseEvent<events::exit>(exitCode);
            return;
        }

        if (instance.m_exit.load(std::memory_order_relaxed))
        {
            log::warn("Engine was already exiting, triggered additional exit event with code {}", exitCode);
            return;
        }

        log::undecoratedInfo("=========================\n"
            "| Shutting down engine. |\n"
            "=========================");

        instance.m_exitCode = exitCode;
        instance.m_exit.store(true, std::memory_order_release);
    }

    bool Scheduler::isExiting()
    {
        return instance.m_exit.load(std::memory_order_relaxed);
    }

    size_type Scheduler::jobPoolSize() noexcept
    {
        return instance.m_jobPoolSize;
    }

    pointer<ProcessChain> Scheduler::createProcessChain(cstring name)
    {
        id_type id = nameHash(name);
        return { &instance.m_processChains.emplace(id, name, id).first.value() };
    }

    pointer<ProcessChain> Scheduler::getChain(id_type id)
    {
        if (instance.m_processChains.contains(id))
            return { &instance.m_processChains.at(id) };
        return { nullptr };
    }

    pointer<ProcessChain> Scheduler::getChain(cstring name)
    {
        id_type id = nameHash(name);
        if (instance.m_processChains.contains(id))
            return { &instance.m_processChains.at(id) };
        return { nullptr };
    }

    void Scheduler::subscribeToThreadCreate(const thread_callback_delegate& callback)
    {
        getOnThreadCreate().push_back(callback);
    }

    void Scheduler::subscribeToChainStart(id_type chainId, const chain_callback_delegate& callback)
    {
        instance.m_processChains.at(chainId).subscribeToChainStart(callback);
    }

    void Scheduler::subscribeToChainStart(cstring chainName, const chain_callback_delegate& callback)
    {
        id_type chainId = nameHash(chainName);
        instance.m_processChains.at(chainId).subscribeToChainStart(callback);
    }

    void Scheduler::unsubscribeFromChainStart(id_type chainId, const chain_callback_delegate& callback)
    {
        instance.m_processChains.at(chainId).unsubscribeFromChainStart(callback);
    }

    void Scheduler::unsubscribeFromChainStart(cstring chainName, const chain_callback_delegate& callback)
    {
        id_type chainId = nameHash(chainName);
        instance.m_processChains.at(chainId).unsubscribeFromChainStart(callback);
    }

    void Scheduler::subscribeToChainEnd(id_type chainId, const chain_callback_delegate& callback)
    {
        instance.m_processChains.at(chainId).subscribeToChainEnd(callback);
    }

    void Scheduler::subscribeToChainEnd(cstring chainName, const chain_callback_delegate& callback)
    {
        id_type chainId = nameHash(chainName);
        instance.m_processChains.at(chainId).subscribeToChainEnd(callback);
    }

    void Scheduler::subscribeToFrameStart(const frame_callback_delegate& callback)
    {
        instance.m_onFrameStart.push_back(callback);
    }

    void Scheduler::unsubscribeFromFrameStart(const frame_callback_delegate& callback)
    {
        instance.m_onFrameStart.erase(callback);
    }

    void Scheduler::subscribeToFrameEnd(const frame_callback_delegate& callback)
    {
        instance.m_onFrameEnd.push_back(callback);
    }

    void Scheduler::unsubscribeFromFrameEnd(const frame_callback_delegate& callback)
    {
        instance.m_onFrameEnd.erase(callback);
    }

    void Scheduler::unsubscribeFromChainEnd(id_type chainId, const chain_callback_delegate& callback)
    {
        instance.m_processChains.at(chainId).unsubscribeFromChainEnd(callback);
    }

    void Scheduler::unsubscribeFromChainEnd(cstring chainName, const chain_callback_delegate& callback)
    {
        id_type chainId = nameHash(chainName);
        instance.m_processChains.at(chainId).unsubscribeFromChainEnd(callback);
    }

    bool Scheduler::hookProcess(cstring chainName, Process& process)
    {
        id_type chainId = nameHash(chainName);

        if (instance.m_processChains.contains(chainId))
        {
            instance.m_processChains[chainId].addProcess(process);
            return true;
        }

        return false;
    }

    bool Scheduler::hookProcess(cstring chainName, pointer<Process> process)
    {
        id_type chainId = nameHash(chainName);

        if (instance.m_processChains.contains(chainId))
        {
            instance.m_processChains[chainId].addProcess(process);
            return true;
        }

        return false;
    }

    bool Scheduler::unhookProcess(cstring chainName, Process& process)
    {
        id_type chainId = nameHash(chainName);

        if (instance.m_processChains.contains(chainId))
            return instance.m_processChains[chainId].removeProcess(process);

        return false;
    }

    bool Scheduler::unhookProcess(cstring chainName, pointer<Process> process)
    {
        id_type chainId = nameHash(chainName);

        if (instance.m_processChains.contains(chainId))
            return instance.m_processChains[chainId].removeProcess(process);

        return false;
    }

    bool Scheduler::unhookProcess(id_type chainId, pointer<Process> process)
    {
        if (instance.m_processChains.contains(chainId))
            return instance.m_processChains[chainId].removeProcess(process);

        return false;
    }

}
