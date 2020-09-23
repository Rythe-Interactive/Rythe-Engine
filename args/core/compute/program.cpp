#include <core/compute/program.hpp>
#include <core/logging/logging.hpp>

#include <core/filesystem/resource.hpp>


namespace args::core::compute {

    Program::Program(cl_context ctx, cl_device_id device, filesystem::basic_resource container) {


        this->make_command_queue = std::function([ctx, device]() -> cl_command_queue
            {
                cl_int ret;
                auto* const command_queue = clCreateCommandQueueWithProperties(ctx, device, nullptr, &ret);
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
        //DEBUG
        ret = clBuildProgram(m_program, 1, &device, "-cl-std=CL2.0 -cl-kernel-arg-info -DARGS_LIBRARY -DDEBUG", nullptr, nullptr);
#else
        //NDEBUG
        ret = clBuildProgram(m_program, 1, &device, "-cl-std=CL2.0 -cl-kernel-arg-info -DARGS_LIBRARY -DNDEBUG", nullptr, nullptr);
#endif
        if (ret != CL_SUCCESS)
        {
            log::error("clBuildProgram failed");
        }
    }

    Kernel Program::kernelContext(const std::string& name)
    {
        // check if we already have the kernel and create a context from it if we do, otherwise initialize it

        if (const auto it = m_kernelCache.find(name); it != m_kernelCache.end())
        {
            return Kernel(this, it->second);
        }
        else
        {
            return Kernel(this, prewarm(name));
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
