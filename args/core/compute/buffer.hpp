#pragma once
#include <memory>
#include <CL/cl.h>

#include <core/platform/platform.hpp>
#include <core/types/primitives.hpp>
#include <tuple>

namespace args::core::compute {

    enum class buffer_type : int
    {
        WRITE_BUFFER = 1,
        READ_BUFFER = 2
    };

    inline buffer_type operator|(const buffer_type& lhs,const buffer_type& rhs)
    {
        return static_cast<buffer_type>(static_cast<int>(lhs) | static_cast<int>(rhs));
    }

    inline buffer_type& operator|=(buffer_type& lhs,const buffer_type& rhs)
    {
        return lhs = lhs | rhs;
    }



    class ARGS_API Buffer
    {
    public:
        Buffer(cl_context,byte*,size_t,buffer_type);

        Buffer(Buffer&& b) noexcept;
        Buffer(const Buffer& b);


        ~Buffer();

        /*
        template <class T>
        std::pair<T*,size_t> read()
        {
            return std::make_pair(reinterpret_cast<T*>(m_data->data()),m_data->size() / sizeof(T));
        }
        */

    private:
        friend class Kernel;
        cl_mem m_memory_object;
        size_t* m_ref_count;
        cl_mem_flags m_type;
        byte*  m_data;
        std::size_t m_size;
    };
}

