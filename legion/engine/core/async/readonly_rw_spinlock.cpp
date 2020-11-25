#include <core/async/readonly_rw_spinlock.hpp>

namespace legion::core::async
{
    std::atomic_uint readonly_rw_spinlock::m_lastId = 1;

    thread_local std::unique_ptr<std::unordered_map<uint, int>> readonly_rw_spinlock::m_localWriters = std::make_unique<std::unordered_map<uint, int>>();
    thread_local std::unique_ptr<std::unordered_map<uint, int>> readonly_rw_spinlock::m_localReaders = std::make_unique<std::unordered_map<uint, int>>();
    thread_local std::unique_ptr<std::unordered_map<uint, lock_state>> readonly_rw_spinlock::m_localState = std::make_unique<std::unordered_map<uint, lock_state>>();
}
