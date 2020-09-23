#include <core/async/readonly_rw_spinlock.hpp>

namespace args::core::async
{
    std::atomic_uint readonly_rw_spinlock::data::m_lastId = 1;
    std::unordered_map<std::thread::id, std::unordered_map<uint, int>> readonly_rw_spinlock::data::m_localWriters;
    std::unordered_map<std::thread::id, std::unordered_map<uint, int>> readonly_rw_spinlock::data::m_localReaders;
    std::unordered_map<std::thread::id, std::unordered_map<uint, lock_state>> readonly_rw_spinlock::data::m_localState;
}
