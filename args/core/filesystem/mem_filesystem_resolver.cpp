#include "mem_filesystem_resolver.hpp"

#include "artifact_cache.hpp"

namespace args::core::filesystem
{
 
    bool mem_filesystem_resolver::seek_data()
    {
        if(has_data_ready())
        {
            artifact_cache::get_data(get_identifier(),&m_data);
            return true;
        }
        else return false;
    }
    bool mem_filesystem_resolver::has_data_ready() const
    {
        artifact_cache::is_cached(get_identifier());
    }

    void mem_filesystem_resolver::set_data(const byte_vec& d)
    {
        m_data = d;
        artifact_cache::set_data(get_identifier(),&m_data);
    }

    const byte_vec& mem_filesystem_resolver::get_data() const
    {
        return m_data;
    }

    byte_vec& mem_filesystem_resolver::get_data()
    {
        return  m_data;
    }
}
