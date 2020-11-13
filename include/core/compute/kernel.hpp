#pragma once
#include "detail/cl_include.hpp"

#include <core/compute/buffer.hpp>
#include <core/logging/logging.hpp>

#include <map>



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
            Kernel(Program*, cl_kernel);

            /**
             * @brief Determines the "Local Work Size", aka how many Kernels should run in parallel
             *         This number should not exceed the amount of available cores, a good max is 1024
             */
            Kernel& local(size_type);

            /**
             * @brief Determines the "Global Work Size", aka how many Kernels should run in total
             *         This number should match your input arrays (but not exceed it!)
             */
            Kernel& global(size_type);

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
             * @brief same as informing the kernel about a buffer and then enqueueing it 
             */
            Kernel& setAndEnqueBuffer(Buffer buffer, block_mode blocking = block_mode::BLOCKING);

            /**
             * @brief same as informing the kernel about a buffer and then enqueueing it 
             */
            Kernel& setAndEnqueBuffer(Buffer buffer, const std::string&, block_mode blocking = block_mode::BLOCKING);

            /**
             * @brief same as informing the kernel about a buffer and then enqueueing it 
             */
            Kernel& setAndEnqueBuffer(Buffer buffer, cl_uint index, block_mode blocking = block_mode::BLOCKING);

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
            size_t getMaxWorkSize() const;

        private:

            buffer_type m_default_mode;
            std::map<std::string, cl_uint> m_paramsMap;
            Program* m_prog;
            cl_kernel m_func;
            cl_command_queue m_queue;
            size_type m_global_size;
            size_type m_local_size;


            //helper function to wrap parameter checking 
            template <class F,class... Args>
            void param_find(F && func,std::string name)
            {
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
