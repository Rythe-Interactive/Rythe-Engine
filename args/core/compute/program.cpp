#include <core/compute/program.hpp>
#include <core/logging/logging.hpp>

#include <core/filesystem/resource.hpp>


namespace args::core::compute {

    Program::Program(cl_context ctx, cl_device_id device, filesystem::basic_resource container) {


        this->make_command_queue = std::function([ctx, device]() -> cl_command_queue
            {
                cl_int ret;
                const auto  command_queue = clCreateCommandQueueWithProperties(ctx, device, nullptr, &ret);
                if (ret != NULL) {
                    log::error("clCreateCommandQueueWithProperties failed!");
                    return nullptr;
                }
                return command_queue;
            });

        cl_int ret;


        const char* data = reinterpret_cast<const char*>(container.data());
        size_t size = container.size();

        m_program = clCreateProgramWithSource(ctx, 1, &data, &size, &ret);
        if (ret != NULL)
        {
            log::error("clCreateProgramWithSource failed!");
            return;
        }

#ifdef ARGS_DEBUG
        ret = clBuildProgram(m_program, 1, &device, "-cl-std=CL2.0 -cl-kernel-arg-info -DARGS_LIBRARY -DDEBUG", nullptr, nullptr);
#else
        ret = clBuildProgram(m_program, 1, &device, "-cl-std=CL2.0 -cl-kernel-arg-info -DARGS_LIBRARY -DNDEBUG", nullptr, nullptr);
#endif
        if (ret != CL_SUCCESS)
        {
            log::error("clBuildProgram failed");
        }
    }

    Program::ExecutionContext& Program::ExecutionContext::build_buffer_names()
    {
        size_t size;
        cl_uint num_args;
        std::string container;


        clGetKernelInfo(m_func, CL_KERNEL_NUM_ARGS, sizeof(cl_uint), &num_args, NULL);

        for (cl_uint i = 0; i < num_args; ++i) {
            if (clGetKernelArgInfo(m_func, i, CL_KERNEL_ARG_NAME, 0, nullptr, &size) == CL_KERNEL_ARG_INFO_NOT_AVAILABLE)
                log::error("cannot get Argument name");
            container.resize(size-1, '\0');
            clGetKernelArgInfo(m_func, i, CL_KERNEL_ARG_NAME, size, container.data(), nullptr);
            m_paramsMap[container] = i;

        }
        return *this;
    }

    Program::ExecutionContext& Program::ExecutionContext::read_write_mode(buffer_type t)
    {
        m_default_type = t;
        return *this;
    }

    Program::ExecutionContext& Program::ExecutionContext::tell_buffer(Buffer buffer, const std::string& name)
    {
        if (m_paramsMap.empty())
        {
            build_buffer_names();
        }

        if (const auto it = m_paramsMap.find(name); it != m_paramsMap.end())
        {
            tell_buffer(std::forward<Buffer>(buffer), it->second);
        }
        return *this;
    }

    Program::ExecutionContext& Program::ExecutionContext::show_buffer(Buffer buffer, const std::string& name, bool blocking)
    {
        if (m_paramsMap.empty())
        {
            build_buffer_names();
        }

        if (const auto it = m_paramsMap.find(name); it != m_paramsMap.end())
        {
            show_buffer(std::forward<Buffer>(buffer), it->second, blocking);
        }
        return *this;
    }

    Program::ExecutionContext& Program::ExecutionContext::tell_buffer(Buffer buffer, size_t index)
    {
        const cl_int ret = clSetKernelArg(m_func, index, sizeof(cl_mem), &buffer.m_memory_object);
        
        if(ret != CL_SUCCESS)
        {
            log::error("clSetKernelArg {}",ret);
        }
        return *this;
    }

    Program::ExecutionContext& Program::ExecutionContext::show_buffer(Buffer buffer, size_t index, bool blocking)
    {
        cl_int ret = 0;
        switch (buffer.m_type)
        {
        case CL_MEM_READ_ONLY:
            ret = clEnqueueWriteBuffer(m_queue, buffer.m_memory_object, blocking, 0, buffer.m_size, buffer.m_data, 0, nullptr, nullptr);
            break;
        case CL_MEM_WRITE_ONLY:
            ret = clEnqueueReadBuffer(m_queue, buffer.m_memory_object, blocking, 0, buffer.m_size, buffer.m_data, 0, nullptr, nullptr);
            break;
        case CL_MEM_READ_WRITE:
            switch (m_default_type)
            {
            case buffer_type::READ_BUFFER:
                ret = clEnqueueWriteBuffer(m_queue, buffer.m_memory_object, blocking, 0, buffer.m_size, buffer.m_data, 0, nullptr, nullptr);
                break;

            case buffer_type::WRITE_BUFFER:
                ret = clEnqueueReadBuffer(m_queue, buffer.m_memory_object, blocking, 0, buffer.m_size, buffer.m_data, 0, nullptr, nullptr);
                break;

            default:
                throw std::logic_error("buffer enqueue type was neither read nor write ... I don't know what to do!");
            }
            break;
        }

        if(ret != CL_SUCCESS)
            log::error("clEnqueueXXXXBuffer {}",ret);

        return *this;
    }


    Program::ExecutionContext& Program::ExecutionContext::enqueue_buffer(Buffer buffer, const std::string& name, bool blocking)
    {
        if (m_paramsMap.empty())
        {
            build_buffer_names();
        }

        if (const auto it = m_paramsMap.find(name); it != m_paramsMap.end())
        {
            enqueue_buffer(std::forward<Buffer>(buffer), it->second, blocking);
        }
        return *this;
    }

    Program::ExecutionContext& Program::ExecutionContext::enqueue_buffer(Buffer buffer, size_t index, bool blocking)
    {
        cl_int ret = 0;
        switch (buffer.m_type)
        {
        case CL_MEM_READ_ONLY:
            ret = clEnqueueWriteBuffer(m_queue, buffer.m_memory_object, blocking, 0, buffer.m_size, buffer.m_data, 0, nullptr, nullptr);
            break;
        case CL_MEM_WRITE_ONLY:
            ret = clEnqueueReadBuffer(m_queue, buffer.m_memory_object, blocking, 0, buffer.m_size, buffer.m_data, 0, nullptr, nullptr);
            break;
        case CL_MEM_READ_WRITE:
            switch (m_default_type)
            {
            case buffer_type::READ_BUFFER:
                ret = clEnqueueWriteBuffer(m_queue, buffer.m_memory_object, blocking, 0, buffer.m_size, buffer.m_data, 0, nullptr, nullptr);
                break;

            case buffer_type::WRITE_BUFFER:
                ret = clEnqueueReadBuffer(m_queue, buffer.m_memory_object, blocking, 0, buffer.m_size, buffer.m_data, 0, nullptr, nullptr);
                break;

            default:
                throw std::logic_error("buffer enqueue type was neither read nor write ... I don't know what to do!");
            }
            break;
        }
        
        if(ret != CL_SUCCESS)
            log::error("clEnqueueXXXXBuffer {}",ret);

        ret = clSetKernelArg(m_func, index, sizeof(cl_mem), &buffer.m_memory_object);
        
        if(ret != CL_SUCCESS)
            log::error("clSetKernelArg {}",ret);
        return *this;
    }


    Program::ExecutionContext& Program::ExecutionContext::dispatch()
    {
        cl_int ret = clEnqueueNDRangeKernel(m_queue, m_func, 1, NULL, &m_global_size, &m_local_size, 0, NULL, NULL);
        if (ret != CL_SUCCESS)
        {
            log::error("clEnqueueNDRangeKernel failed: {}", ret);
        }
        return *this;
    }

    void Program::ExecutionContext::finish() const
    {
        clFlush(m_queue);
        clFinish(m_queue);
        clReleaseCommandQueue(m_queue);
    }


    Program::ExecutionContext::ExecutionContext(Program* program, cl_kernel kernel) :
        m_prog(program),
        m_func(kernel),
        m_default_type(buffer_type::READ_BUFFER),
        m_local_size(64),
        m_global_size(0)
    {
        cl_int ret;
        m_queue = program->make_command_queue();

    }

    Program::ExecutionContext& Program::ExecutionContext::local(size_t s)
    {
        m_local_size = s;
        return *this;
    }

    Program::ExecutionContext& Program::ExecutionContext::global(size_t s)
    {
        m_global_size = s;
        return *this;

    }


    Program::ExecutionContext Program::functionContext(const std::string& name)
    {
        // check if we already have the kernel and create a context from it if we do, otherwise initialize it

        if (const auto it = m_kernelCache.find(name); it != m_kernelCache.end())
        {
            return ExecutionContext(this, it->second);
        }
        else
        {
            return ExecutionContext(this, prewarm(name));
        }


    }

    cl_kernel Program::prewarm(const std::string& name)
    {
        // create the kernel and push it into the cache
        // do some error checking along the way

        cl_int ret;
        cl_kernel kernel = clCreateKernel(m_program, name.c_str(), &ret);
        if (ret != NULL)
        {
            log::error("clCreateKernel failed");
        }


        m_kernelCache[name] = kernel;
        return kernel;
    }
}
