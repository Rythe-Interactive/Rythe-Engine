#include <core/compute/buffer.hpp>
#include <core/logging/logging.hpp>


namespace args::core::compute
{
    bool operator&(const buffer_type& lhs, const buffer_type& rhs)
    {
        return static_cast<int>(lhs) & static_cast<int>(rhs);
    }

    Buffer::Buffer(cl_context ctx,byte* data,size_t len, buffer_type type,std::string name) : m_data(data) , m_size(len) , m_name(std::move(name))
    {
    	//initialize new ref-counter
        m_ref_count = new size_t(1);


    	//convert buffer_type to cl_mem_flags
        if (type == buffer_type::READ_BUFFER)
            m_type = CL_MEM_READ_ONLY;
        else if (type == buffer_type::WRITE_BUFFER)
            m_type = CL_MEM_WRITE_ONLY;
        else
            m_type = CL_MEM_READ_WRITE;
            

        cl_int ret;

    	//create buffer
        m_memory_object = clCreateBuffer(ctx,m_type,m_size,nullptr,&ret);
        if(ret != NULL)
        {
            log::error("clCreateBuffer failed for Buffer: {}", name);
        }

    }

    void Buffer::rename(const std::string& name)
    {
    	m_name = name;
    }
	
    Buffer::Buffer(Buffer&& b) noexcept :
        m_name(std::move(b.m_name)),
		m_memory_object(b.m_memory_object),
        m_ref_count(b.m_ref_count),
        m_type(b.m_type),
        m_data(b.m_data),
        m_size(b.m_size)
    {

    	//Move Ctor needs to be explicitly defined
    	//to increase Reference Counter
        ++*m_ref_count;
    }

    Buffer::Buffer(const Buffer& b) :
        m_name(b.m_name),
        m_memory_object(b.m_memory_object),
        m_ref_count(b.m_ref_count),
        m_type(b.m_type),
        m_data(b.m_data),
        m_size(b.m_size)
    {
    	//Copy Ctor needs to be explicitly defined
    	//to increase Reference Counter
        ++*m_ref_count;
    }




    Buffer::~Buffer()
    {

    	//check if this is the last element
    	//TODO(algo-ryth-mix): make this thread-safe
        if(--*m_ref_count == 0)
        {
        	//free ref-counter & memory_object
            delete m_ref_count;
            clReleaseMemObject(m_memory_object);
        }
    }
}
