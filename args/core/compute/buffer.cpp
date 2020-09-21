#include <core/compute/buffer.hpp>
#include <core/logging/logging.hpp>


namespace args::core::compute
{
    bool operator&(const buffer_type& lhs, const buffer_type& rhs)
    {
        return static_cast<int>(lhs) & static_cast<int>(rhs);
    }

    Buffer::Buffer(cl_context ctx,byte* data,size_t len, buffer_type type) : m_data(data) , m_size(len)
    {
        m_ref_count = new size_t(1);

        if (type == buffer_type::READ_BUFFER)
            m_type = CL_MEM_READ_ONLY;
        else if (type == buffer_type::WRITE_BUFFER)
            m_type = CL_MEM_WRITE_ONLY;
        else
            m_type = CL_MEM_READ_WRITE;
            

        cl_int ret;
        m_memory_object = clCreateBuffer(ctx,m_type,m_size,nullptr,&ret);
        if(ret != NULL)
        {
            log::error("clCreateBuffer failed");
        }

    }

    Buffer::Buffer(Buffer&& b) noexcept :
        m_memory_object(b.m_memory_object),
        m_ref_count(b.m_ref_count),
        m_type(b.m_type),
        m_data(b.m_data),
        m_size(b.m_size)
    {
        ++*m_ref_count;
    }

    Buffer::Buffer(const Buffer& b) :
        m_memory_object(b.m_memory_object),
        m_ref_count(b.m_ref_count),
        m_type(b.m_type),
        m_data(b.m_data),
        m_size(b.m_size)
    {
        ++*m_ref_count;
    }


    Buffer::~Buffer()
    {

        if(--*m_ref_count == 0)
        {
            delete m_ref_count;
            clReleaseMemObject(m_memory_object);
        }
    }
}
