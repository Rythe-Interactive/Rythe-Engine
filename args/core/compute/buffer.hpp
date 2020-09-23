#pragma once
#include <CL/cl.h> // cl_context , cl_mem , cl_mem_flags

#include <core/platform/platform.hpp> // ARGS_API
#include <core/types/primitives.hpp> // byte, size_t

#include <tuple> // pair
#include <string> // string

/** 
 * @file buffer.hpp
 */

namespace args::core::compute {


    /**
     * @brief Describes what kind of compute buffer you want,
     *        Can be either READ, WRITE or READ | WRITE
     *        Note that if your buffer is READ | WRITE you need
     *        To specifiy manually what operation you want to
     *        perform
     */
    enum class buffer_type : int
    {
        WRITE_BUFFER = 1,
        READ_BUFFER = 2
    };

    /**
     * @brief Bitwise or operator for type: buffer_type
     */
    inline buffer_type operator|(const buffer_type& lhs,const buffer_type& rhs)
    {
        return static_cast<buffer_type>(static_cast<int>(lhs) | static_cast<int>(rhs));
    }

    /**
     * @brief Bitwise or assignment operator for type: buffer_type
     */
    inline buffer_type& operator|=(buffer_type& lhs,const buffer_type& rhs)
    {
        return lhs = lhs | rhs;
    }


    /**
     * @class Buffer
     * @brief Wraps an OpenCL buffer in a more "User-Friendly Interface"
     *        You would normally obtain these via
     *        @ref Context::createBuffer()
     */
    class ARGS_API Buffer
    {
    public:
        Buffer(cl_context,byte*,size_t,buffer_type,std::string);

        Buffer(Buffer&& b) noexcept;
        Buffer(const Buffer& b);

        //copy assign & move assign operator
        //are deleted since the Buffer
        //refcounts
        Buffer& operator=(Buffer&&) noexcept = delete;
        Buffer& operator=(const Buffer&) = delete;

        void rename(const std::string& name);

        ~Buffer();
    private:
        friend class Program;
        friend class Kernel;

        std::string m_name;
        cl_mem m_memory_object;
        size_type* m_ref_count;
        cl_mem_flags m_type;
        byte*  m_data;
        size_type m_size;
    };
}

