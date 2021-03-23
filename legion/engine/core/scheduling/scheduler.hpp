#pragma once
#include <thread>

#include <Optick/optick.h>

#include <core/async/spinlock.hpp>
#include <core/containers/sparse_map.hpp>

#include <core/scheduling/processchain.hpp>

namespace legion::core::scheduling
{
    const inline static std::thread::id invalid_thread_id = std::thread::id();

    class Scheduler
    {
    private:
#if USE_OPTICK
        async::spinlock m_threadScopesLock;
        std::vector<std::unique_ptr<Optick::ThreadScope>> m_threadScopes;
#endif
        sparse_map<id_type, ProcessChain> m_processChains;

    public:
    };
}
