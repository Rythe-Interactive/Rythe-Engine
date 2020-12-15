#include <core/async/rw_spinlock.hpp>

namespace legion::core::async
{
    bool rw_spinlock::m_forceRelease = false;
    std::atomic_uint rw_spinlock::m_lastId = { 1 };

    thread_local std::unique_ptr<std::unordered_map<uint, int>> rw_spinlock::m_localWriters = std::make_unique<std::unordered_map<uint, int>>();
    thread_local std::unique_ptr<std::unordered_map<uint, int>> rw_spinlock::m_localReaders = std::make_unique<std::unordered_map<uint, int>>();
    thread_local std::unique_ptr<std::unordered_map<uint, lock_state>> rw_spinlock::m_localState = std::make_unique<std::unordered_map<uint, lock_state>>();
}
