#include "mem_filesystem_resolver.hpp"

#include <utility>

#include "artifact_cache.hpp"

namespace legion::core::filesystem
{
    mem_filesystem_resolver::mem_filesystem_resolver(std::shared_ptr<const byte_vec> target_data)
        : m_targetData(std::move(target_data))
    {
    }

    bool mem_filesystem_resolver::prewarm() const
    {
        //check if data is already available
        if(!m_data)
            m_data = artifact_cache::get_cache(get_identifier(),size_hint(m_targetData));

        //check if data was already built
        if(m_data->empty())
        {
            //check if building information is available
            if(!m_targetData) return false;

            //build faster representation
            build_memory_representation(m_targetData,m_data);
        }
        return true;
    }

    const byte_vec& mem_filesystem_resolver::get_data() const
    {
        if(!prewarm()) throw legion_fs_error("attempting to access data via a purely cached object, which was not cached");
        return *m_data;
    }

    byte_vec& mem_filesystem_resolver::get_data()
    {
        if(!prewarm()) throw legion_fs_error("attempting to access data via a purely cached object, which was not cached");
        return *m_data;
    }
}
