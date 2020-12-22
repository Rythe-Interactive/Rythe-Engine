#pragma once
#include "detail/cl_include.hpp"

#include <core/compute/buffer.hpp>
#include <core/logging/logging.hpp>
#include <variant>
#include <map>

#include <Optick/optick.h>

namespace legion::core::compute
{
    enum class block_mode : bool {
        BLOCKING,
        NON_BLOCKING
    };

    class Program;

    /**
     * @class Kernel
     * @brief Wraps a cl_kernel, allows you to parametrize an
     *         OpenCL kernel without worrying about the CommandQueue
     */
    class Kernel
    {
    public:
        struct d2
        {
            size_type s0;
            size_type s1;
        };

        struct d3
        {
            size_type s0;
            size_type s1;
            size_type s3;
        };

        using dimension = std::variant<size_type,d2,d3>;

        Kernel(Program*, cl_kernel);

        Kernel(const Kernel& other)
            : m_refcounter(other.m_refcounter),
              m_default_mode(other.m_default_mode),
              m_paramsMap(other.m_paramsMap),
              m_prog(other.m_prog),
              m_func(other.m_func),
              m_queue(other.m_queue),
              m_global_size(other.m_global_size),
              m_local_size(other.m_local_size)
        {
            if(m_refcounter)++*m_refcounter;
        }

        Kernel(Kernel&& other) noexcept
            : m_refcounter(other.m_refcounter),
              m_default_mode(other.m_default_mode),
              m_paramsMap(std::move(other.m_paramsMap)),
              m_prog(other.m_prog),
              m_func(other.m_func),
              m_queue(other.m_queue),
              m_global_size(std::move(other.m_global_size)),
              m_local_size(std::move(other.m_local_size))
        {
            if(m_refcounter)++*m_refcounter;
        }

        Kernel& operator=(const Kernel& other)
        {
            OPTICK_EVENT();
            if (this == &other)
                return *this;
            m_refcounter = other.m_refcounter;
            m_default_mode = other.m_default_mode;
            m_paramsMap = other.m_paramsMap;
            m_prog = other.m_prog;
            m_func = other.m_func;
            m_queue = other.m_queue;
            m_global_size = other.m_global_size;
            m_local_size = other.m_local_size;
            if(m_refcounter) ++*m_refcounter;
            return *this;
        }

        Kernel& operator=(Kernel&& other) noexcept
        {
            OPTICK_EVENT();
            if (this == &other)
                return *this;
            m_refcounter = other.m_refcounter;
            m_default_mode = other.m_default_mode;
            m_paramsMap = std::move(other.m_paramsMap);
            m_prog = other.m_prog;
            m_func = other.m_func;
            m_queue = other.m_queue;
            m_global_size = std::move(other.m_global_size);
            m_local_size = std::move(other.m_local_size);
            if(m_refcounter)++*m_refcounter;
            return *this;
        }
        ~Kernel()
        {
            OPTICK_EVENT();
            if(m_refcounter)--*m_refcounter;
            if(m_refcounter && *m_refcounter == 0)
            {
                delete m_refcounter;
                clReleaseCommandQueue(m_queue);
            }
        }

        /**
         * @brief Determines the "Local Work Size", aka how many Kernels should run in parallel
         *         This number should not exceed the amount of available cores, a good max is 1024
         */
        Kernel& local(size_type);

        /**
         * @brief Determines the "Global Work Size", aka how many Kernels should run in total
         *         This number should match your input arrays (but not exceed it!)
         */
        Kernel& global(dimension);
        Kernel& global(size_type,size_type);
        Kernel& global(size_type,size_type,size_type);

        /**
         * @brief Maps Parameter Names to indices, for example:
         * @code
         *  __kernel void do_something(__global int* Parameter1, __global int* Parameter2) { ... }
         * @endcode
         * would be mapped to:
         *  - Parameter1 -> 0
         *  - Parameter2 -> 1
         */
        Kernel& buildBufferNames();

        /**
         * @brief Sets the modus operandi for Read-Write Buffers:
         * - if you set read_write_mode to READ_BUFFER the next Read-Write Buffer will be read
         * - if you set read_write_mode to WRITE_BUFFER the next Read-Writer Buffer will be written
         * - setting it to READ | WRITE does not work and will throw a logic_error exception when
         *   trying to enqueue
         */
        Kernel& readWriteMode(buffer_type);

        /**
         * @brief Enqueues a Buffer in the command queue, write buffers need to be enqueued before
         *         dispatching the kernel, read buffers afterwards
         * @param buffer The buffer you want to enqueue.
         * @param index The index of the Buffer you want to enqueue to (this must match the kernel index)
         * @param blocking Whether or not you want a blocking write/read (default=BLOCKING)
         */
        Kernel& enqueueBuffer(Buffer buffer, block_mode blocking = block_mode::BLOCKING);

        /**
         * @brief Sets a Buffer as the Kernel Argument
         * @note this is required for all Kernel Arguments regardless of buffer direction
         * @note this needs to be done for all buffers before the kernel is dispatched
         * @param buffer The buffer you want to inform the kernel about
         * @note This Overload requires that the buffer is named
         */
        Kernel& setBuffer(Buffer buffer);

        /**
         * @brief Sets a Buffer as the Kernel Argument
         * @note this is required for all Kernel Arguments regardless of buffer direction
         * @note this needs to be done for all buffers before the kernel is dispatched
         * @param buffer The buffer you want to inform the kernel about
         * @param name The name of the Buffer you want to inform the kernel about
         */
        Kernel& setBuffer(Buffer buffer, const std::string& name);

        /**
         * @brief Sets a Buffer as the Kernel Argument
         * @note this is required for all Kernel Arguments regardless of buffer direction
         * @note this needs to be done for all buffers before the kernel is dispatched
         * @param buffer The buffer you want to inform the kernel about
         * @param index The index of the Buffer you want to inform the kernel about
         */
        Kernel& setBuffer(Buffer buffer, cl_uint index);


        /**
         * @brief Sets a Kernel Parameter that is the same for all invocations of the kernel.
         * @tparam T The type of the Kernel Argument.
         * @param value A pointer to the value of the kernel Argument.
         * @param name The name of the Kernel Argument as it appears in the Kernel.
         */
        template <class T>
        Kernel& setKernelArg(T* value, const std::string& name)
        {
            OPTICK_EVENT();
            return setKernelArg(value,sizeof(T),name);
        }

        /**
         * @brief Sets a Kernel Parameter that is the same for all invocations of the kernel.
         * @tparam T The type of the Kernel Argument.
         * @param value A pointer to the value of the kernel Argument.
         * @param index The index of the parameter.
         */
        template <class T>
        Kernel& setKernelArg(T* value, cl_uint index)
        {
            OPTICK_EVENT();
            return setKernelArg(value,sizeof(T),index);
        }
        

        /**
         * @brief See above
         * @param value Value of the parameter.
         * @param size  Sizeof of the parameter.
         * @param name The name of the Kernel Argument as it appears in the Kernel.
         */
        Kernel& setKernelArg(void* value, size_type size, const std::string& name);

        /**
         * @brief See above
         * @param value Value of the parameter.
         * @param size  Sizeof of the parameter.
         * @param index The index of the parameter.
         */
        Kernel& setKernelArg(void* value, size_type size, cl_uint index);

        /**
         * @brief same as informing the kernel about a buffer and then enqueueing it 
         */
        Kernel& setAndEnqueueBuffer(Buffer buffer, block_mode blocking = block_mode::BLOCKING);

        /**
         * @brief same as informing the kernel about a buffer and then enqueueing it 
         */
        Kernel& setAndEnqueueBuffer(Buffer buffer, const std::string&, block_mode blocking = block_mode::BLOCKING);

        /**
         * @brief same as informing the kernel about a buffer and then enqueueing it 
         */
        Kernel& setAndEnqueueBuffer(Buffer buffer, cl_uint index, block_mode blocking = block_mode::BLOCKING);

        /**
         * @brief Dispatches the Kernel to the command-queue
         * @note All Kernel Parameters must be set before this can be called
         * @note global and local must be set before this can be called
         * @pre set_buffer
         * @pre set_and_enqueue_buffer
         * @pre local
         * @pre global
         */
        Kernel& dispatch();

        /**
         * @brief Ensures that the command-queue is committed and finished executing
         * @pre dispatch
         */
        void finish() const;


        /**
         * @brief Gets the maximum parallel work size for this kernel.
         */
        size_type getMaxWorkSize() const;

    private:

        size_t* m_refcounter;

        buffer_type m_default_mode;
        std::map<std::string, cl_uint> m_paramsMap;
        Program* m_prog;
        cl_kernel m_func;
        cl_command_queue m_queue;

        std::tuple<std::vector<size_type>,std::vector<size_type>,size_type> parse_dimensions()
        {
            OPTICK_EVENT();
            size_type dim = 1;
            size_type* v;
            if((v = reinterpret_cast<size_type*>(std::get_if<d3>(&m_global_size))))
            {
                dim = 3;
            }
            else if ((v = reinterpret_cast<size_type*>(std::get_if<d2>(&m_global_size))))
            {
                dim  = 2;
            }
            else
            {
                v = &std::get<size_type>(m_global_size);
            }

            std::vector<size_type> l;
            for (size_type i = 0; i < dim; ++i)
            {
                l.emplace_back(m_local_size / dim);
            }

            return std::make_tuple(std::vector<size_type>(v,v+dim),l,dim);
        }

        dimension m_global_size;
        size_type m_local_size;


        //helper function to wrap parameter checking 
        template <class F,class... Args>
        void param_find(F && func,std::string name)
        {
            OPTICK_EVENT();
            // lazy build buffer names
            if (m_paramsMap.empty())
            {
                buildBufferNames();
            }

            //find and do thing, otherwise print warning
            if (const auto it = m_paramsMap.find(name); it != m_paramsMap.end())
            {
                std::invoke(func,it->second);
            }
            else
            {
                log::warn("Encountered buffer with name: \"{}\", which was not found as a kernel parameter", name);
            }
        }
    };
}
