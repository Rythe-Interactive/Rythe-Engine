#include <core/compute/context.hpp>
#include <core/logging/logging.hpp>

#include <string>

namespace legion::core::compute {

	//Initialize static variables
    bool Context::m_initialized = false;
    cl_context Context::m_context = nullptr;
    cl_platform_id Context::m_platform_id = nullptr;
    cl_device_id Context::m_device_id = nullptr;


    void Context::init()
    {
        OPTICK_EVENT();
        if(m_initialized) return; // if the context already exists do not initialize

        cl_uint ret_num_devices;
        cl_uint ret_num_platforms;

        //get a suitable computing platform
        cl_int ret = clGetPlatformIDs(1, &m_platform_id, &ret_num_platforms);

    	//error checking for clGetPlatformIDs
        if (ret != CL_SUCCESS)
        {
            log::error("clGetPlatformIDs failed: {}", ret == CL_INVALID_VALUE ? "CL_INVALID_VALUE (params are bad)" : "CL_OUT_OF_HOST_MEMORY");
            return;
        }


        //get a suitable computing device (this should find the best device in the average pc)
        ret = clGetDeviceIDs(m_platform_id, CL_DEVICE_TYPE_DEFAULT, 1, &m_device_id, &ret_num_devices);

    	//error checking for clGetDeviceIDs
        if (ret != CL_SUCCESS)
        {
            std::string error = "Unknown Error";
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
        m_context = clCreateContext(nullptr, 1, &m_device_id, nullptr, nullptr, &ret);

    	//error checking for clCreateContext
        if (ret != CL_SUCCESS)
        {
            std::string error = "Unknown Error" + std::to_string(ret);
            switch (ret)
            {
            case CL_INVALID_PLATFORM:       error = "CL_INVALID_PLATFORM"; break;
            case CL_INVALID_PROPERTY:       error = "CL_INVALID_PLATFORM";break;
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

    	//if everything works out, we can now assume that the context is initialized
        m_initialized = true;
    }

    bool Context::initialized()
    {
        return m_initialized;
    }

    Program Context::createProgram(const filesystem::basic_resource& data)
    {
        OPTICK_EVENT();
        return Program(m_context,m_device_id,data);
    }
}
