#include <core/compute/context.hpp>
#include <core/logging/logging.hpp>

#include <string>

namespace legion::core::compute {

    //Initialize static variables

    void Context::onInit()
    {
        create();

        cl_uint ret_num_devices;
        cl_uint ret_num_platforms;

        //get a suitable computing platform
        cl_int ret = clGetPlatformIDs(1, &instance.m_platform_id, &ret_num_platforms);

        //error checking for clGetPlatformIDs
        if (ret != CL_SUCCESS)
        {
            log::error("clGetPlatformIDs failed: {}", ret == CL_INVALID_VALUE ? "CL_INVALID_VALUE (params are bad)" : "CL_OUT_OF_HOST_MEMORY");
            return;
        }


        //get a suitable computing device (this should find the best device in the average pc)
        ret = clGetDeviceIDs(instance.m_platform_id, CL_DEVICE_TYPE_DEFAULT, 1, &instance.m_device_id, &ret_num_devices);

        //error checking for clGetDeviceIDs
        if (ret != CL_SUCCESS)
        {
            std::string error = "Unknown Error " + std::to_string(ret);
            switch (ret)
            {
            case CL_INVALID_PLATFORM:    error = "CL_INVALID_PLATFORM"; break;
            case CL_INVALID_DEVICE_TYPE: error = "CL_INVALID_DEVICE_TYPE"; break;
            case CL_INVALID_VALUE:       error = "CL_INVALID_VALUE"; break;
            case CL_DEVICE_NOT_FOUND:    error = "CL_DEVICE_NOT_FOUND"; break;
            case CL_OUT_OF_RESOURCES:    error = "CL_OUT_OF_RESOURCES"; break;
            case CL_OUT_OF_HOST_MEMORY:  error = "CL_OUT_OF_HOST_MEMORY"; break;
            default: break;
            }

            log::error("clGetDeviceIDs failed: {}", error);
            return;
        }


        //create the computing context
        instance.m_context = clCreateContext(nullptr, 1, &instance.m_device_id, nullptr, nullptr, &ret);

        //error checking for clCreateContext
        if (ret != CL_SUCCESS)
        {
            std::string error = "Unknown Error " + std::to_string(ret);
            switch (ret)
            {
            case CL_INVALID_PLATFORM:       error = "CL_INVALID_PLATFORM"; break;
            case CL_INVALID_PROPERTY:       error = "CL_INVALID_PLATFORM"; break;
            case CL_INVALID_DEVICE:         error = "CL_INVALID_DEVICE"; break;
            case CL_INVALID_VALUE:          error = "CL_INVALID_VALUE"; break;
            case CL_DEVICE_NOT_AVAILABLE:   error = "CL_DEVICE_NOT_FOUND"; break;
            case CL_OUT_OF_RESOURCES:       error = "CL_OUT_OF_RESOURCES"; break;
            case CL_OUT_OF_HOST_MEMORY:     error = "CL_OUT_OF_HOST_MEMORY"; break;
            default: break;
            }

            log::error("clCreateContext failed: {}", error);
            return;
        }
    }

    void Context::onShutdown()
    {
        auto ret = clReleaseContext(instance.m_context);

        if (ret != CL_SUCCESS)
        {
            std::string error = "Unknown Error " + std::to_string(ret);
            switch (ret)
            {
            case CL_INVALID_CONTEXT:       error = "CL_INVALID_CONTEXT "; break;
            default: break;
            }

            log::error("clReleaseContext failed: {}", error);
        }

        ret = clReleaseDevice(instance.m_device_id);

        if (ret != CL_SUCCESS)
        {
            std::string error = "Unknown Error " + std::to_string(ret);
            switch (ret)
            {
            case CL_INVALID_DEVICE:      error = "CL_INVALID_DEVICE"; break;
            case CL_OUT_OF_RESOURCES:    error = "CL_OUT_OF_RESOURCES"; break;
            case CL_OUT_OF_HOST_MEMORY:  error = "CL_OUT_OF_HOST_MEMORY"; break;
            default: break;
            }

            log::error("clReleaseDevice failed: {}", error);
        }
    }

    Program Context::createProgram(const filesystem::basic_resource& data)
    {
        OPTICK_EVENT();
        return Program(instance.m_context, instance.m_device_id, data);
    }
}
