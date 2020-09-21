#pragma once
#include <functional>
#include <map>
#include <CL/cl.h>

#include <core/filesystem/resource.hpp>
#include <core/compute/buffer.hpp>
#include <string>

namespace args::core::compute {


    class ARGS_API Kernel
    {
    public:

        class ARGS_API ExecutionContext
        {
        public:
            ExecutionContext(Kernel*, cl_kernel);

            ExecutionContext& local(size_t);
            ExecutionContext& global(size_t);

            ExecutionContext& build_buffer_names();
            ExecutionContext& read_write_mode(buffer_type);

            ExecutionContext& tell_buffer(Buffer, const std::string&);
            ExecutionContext& show_buffer(Buffer, const std::string&, bool blocking = true);

            ExecutionContext& tell_buffer(Buffer, size_t index);
            ExecutionContext& show_buffer(Buffer, size_t index, bool blocking = true);


            ExecutionContext& enqueue_buffer(Buffer, const std::string&, bool blocking = true);
            ExecutionContext& enqueue_buffer(Buffer, size_t index, bool blocking = true);
            ExecutionContext& dispatch();
            void finish() const;



        private:

            buffer_type m_default_type;
            std::map<std::string, cl_uint> m_paramsMap;
            Kernel* m_prog;
            cl_kernel m_func;
            cl_command_queue m_queue;
            size_t m_global_size;
            size_t m_local_size;

        };


        Kernel(cl_context, cl_device_id, filesystem::basic_resource);


        Kernel(const Kernel& other) = default;
        Kernel(Kernel&& other) noexcept = default;
        Kernel& operator=(const Kernel& other) = default;
        Kernel& operator=(Kernel&& other) noexcept = default;

        ExecutionContext functionContext(const std::string& name);

        cl_kernel prewarm(const std::string& name);


    private:
        std::function<cl_command_queue()> make_command_queue;
        cl_program m_program;
        std::unordered_map<std::string, cl_kernel> m_kernelCache;
    };

    /*
    inline void test()
    {
        auto kernel = Context::createKernel(filesystem::basic_resource{ nullptr });
        auto ctx = kernel.functionContext("vector_add");

        const auto bufferA = Context::createBuffer({},buffer_type::READ_BUFFER);
        const auto bufferB = Context::createBuffer({},buffer_type::READ_BUFFER);

        const auto writeBuffer = Context::createBuffer({},buffer_type::WRITE_BUFFER);

        ctx
            .build_buffer_names()
            .local(64)
            .global(1024)
            .enqueue_buffer(bufferA, "vectorA")
            .enqueue_buffer(bufferB, "vectorB")
            .dispatch()
            .enqueue_buffer(writeBuffer, "outVec")
            .finish();


    }
    */


}
