#include <core/async/readonly_rw_spinlock.hpp>

namespace args::core::async
{
    std::atomic_uint readonly_rw_spinlock::m_lastId = 1;
    thread_local std::unordered_map<uint, int> readonly_rw_spinlock::m_localWriters;
    thread_local std::unordered_map<uint, int> readonly_rw_spinlock::m_localReaders;
    thread_local std::unordered_map<uint, lock_state> readonly_rw_spinlock::m_localState;
}
