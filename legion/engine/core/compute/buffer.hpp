#pragma once

#include "detail/cl_include.hpp" // cl_context , cl_mem , cl_mem_flags
#include <core/types/primitives.hpp> // byte, size_t

#include <string> // string

/** 
 * @file buffer.hpp
 */

namespace legion::core::compute {


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
    class Buffer
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


        /**
         * @brief Checks if the buffer is named.
         */
        bool hasName() const
        {
            return !m_name.empty(); 
        }

        /**
         * @brief Checks if OpenCL can read from this buffer. 
         */
        bool isReadBuffer() const { return m_type == CL_MEM_READ_ONLY || m_type == CL_MEM_READ_WRITE; }

        /**
         * @brief Checks if OpenCL can write to this buffer.
         */
        bool isWriteBuffer()const  { return m_type == CL_MEM_WRITE_ONLY || m_type == CL_MEM_READ_WRITE; }
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

