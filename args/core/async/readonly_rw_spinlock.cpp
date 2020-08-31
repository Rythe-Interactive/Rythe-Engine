#include <core/async/readonly_rw_spinlock.hpp>

namespace args::core::async
{
    std::atomic_uint readonly_rw_spinlock::data::m_lastId = 1;
}
