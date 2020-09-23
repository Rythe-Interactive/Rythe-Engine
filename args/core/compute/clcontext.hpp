#pragma once
#include <CL/cl.h>

#include <core/platform/platform.hpp>

#include <core/compute/kernel.hpp>

#include <core/filesystem/resource.hpp>

namespace args::core::compute {


class ARGS_API Context
{
public:
    static void create();

    static bool initialized();

    static Kernel createKernel(const filesystem::basic_resource& vec);

    template <class T>
    static Buffer createBuffer(std::vector<T>& container,buffer_type type,std::string name = "Unamed")
    {
        return Buffer(m_context,reinterpret_cast<byte*>(container.data()),container.size() * sizeof( T ),type,std::forward<std::string>(name));
    }


    //static Buffer createBuffer(const byte_vec& v,buffer_type);

private:

    static bool m_initialized;
    static cl_context m_context;
    static cl_platform_id m_platform_id;
    static cl_device_id m_device_id;

};




}
