#include <core/compute/program.hpp>
#include <core/logging/logging.hpp>

#include <core/filesystem/resource.hpp>
#include <core/compute/context.hpp>

namespace legion::core::compute {


    /**
     * Sadly it seems at though the current nVidia driver does not support clCreateProgramWithIL
     */
    Program::Program(cl_context ctx, cl_device_id device, filesystem::basic_resource container /* , bool source_is_il */) {
        OPTICK_EVENT();
        //bind command queue creation to surrogate
        this->make_command_queue = std::function([ctx, device]() -> cl_command_queue
            {
                cl_int ret;
                //creates a command queue and checks it for errors
                auto* const command_queue = clCreateCommandQueueWithProperties(ctx, device, nullptr, &ret);
                if (ret != CL_SUCCESS) {
                    log::error("clCreateCommandQueueWithProperties failed!");
                    return nullptr;
                }
                return command_queue;
            });

        cl_int ret;

        //convert to c-style array
        const char* data = reinterpret_cast<const char*>(container.data());
        size_t size = container.size();

        /*
        if (source_is_il)
        {
            //create program from il
            m_program = clCreateProgramWithIL(ctx, data, size, &ret);
        }
        else { */

            //create program from source
            m_program = clCreateProgramWithSource(ctx, 1, &data, &size, &ret);
        /* } */
        if (ret != CL_SUCCESS)
        {
            log::error("clCreateProgramWithSource/IL failed!");
            return;
        }


        // clBuildProgram parameters guide:
        //
        // -cl-std=2.0: We want OpenCL Standard 2.0 the driver reports 1.2 but it actually is 2.0 on most devices
        // -cl-kernel-arg-info: We want kernel informations built into the binary so that we can query the kernel args by name instead of index
        // -DLEGION_LIBRARY this is indicates to your kernel that it was built for use with the ARGS-Engine, it defines the macor LEGION_LIBRARY
        // -DDEBUG if the Engine is built in debug mode, the kernel  will also receive the DEBUG define
        // -DNDEBUG if the Engine is built in release mode, the kernel will receive the NDEBUG define

        /*if (!source_is_il) {*/

        //check if we are running in debug and adjust build command accordingly
        if constexpr (LEGION_CONFIGURATION == LEGION_DEBUG_VALUE) {
            //DEBUG
            ret = clBuildProgram(m_program, 1, &device, "-cl-std=CL2.0 -cl-kernel-arg-info -DLEGION_LIBRARY -DDEBUG ", nullptr, nullptr);
        } else {
            //NDEBUG
            ret = clBuildProgram(m_program, 1, &device, "-cl-std=CL2.0 -cl-kernel-arg-info -DLEGION_LIBRARY -DNDEBUG", nullptr, nullptr);
        }

        //check if building was successful
        if (ret != CL_SUCCESS)
        {
            log::error("clBuildProgram failed");

            size_t length;
            char buffer[8192*4];

            clGetProgramBuildInfo(m_program,device,CL_PROGRAM_BUILD_LOG,sizeof(buffer),buffer,&length);
            buffer[length] = NULL;
            log::warn("BUILD LOG:\n{}",buffer);
        }

        /*}*/
    }

    Kernel Program::kernelContext(const std::string& name)
    {
        OPTICK_EVENT();
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
        OPTICK_EVENT();
        // create the kernel and push it into the cache
        // do some error checking along the way

        cl_int ret;
        cl_kernel kernel = clCreateKernel(m_program, name.c_str(), &ret);
        if (ret != CL_SUCCESS)
        {
            log::error("clCreateKernel failed");
        }


        m_kernelCache[name] = kernel;
        return kernel;
    }

    void Program::from_resource(Program* value, const filesystem::basic_resource& resource)
    {
        OPTICK_EVENT();
        *value = Context::createProgram(resource);
    }
}
