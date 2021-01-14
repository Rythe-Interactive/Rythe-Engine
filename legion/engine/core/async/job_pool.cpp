#include <core/async/job_pool.hpp>

namespace legion::core::async
{
    thread_local id_type this_job::m_id;

    id_type this_job::get_id() noexcept
    {
        return m_id;
    }

}
