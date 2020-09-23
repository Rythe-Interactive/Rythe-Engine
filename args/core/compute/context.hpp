#pragma once
#include <CL/cl.h>

#include <core/platform/platform.hpp> // ARGS_API
#include <core/compute/program.hpp> // Kernel, Buffer
#include <core/filesystem/resource.hpp> // basic_resource

/**
 * @file context.hpp
 */


namespace args::core::compute {


/** @class Context
 *  @brief Wraps a cl_context in a neater interface.
 */
class ARGS_API Context
{
public:

	/**
     * @brief Initializes the OpenCL context.
     * @pre initialized
     */
    static void init();

    /**
     * @brief Checks if the OpenCL context is initialized.
     */
    static bool initialized();

    /**
     * @brief Creates a Program from a basic resource (can be file or buffer).
     * @param vec The Buffer you want to create the Program from
     *         You can for instance use fs::view.get()
     * @return A Program Object that supports execution
     */
    static Program createProgram(const filesystem::basic_resource& vec);


    /**
     * @brief Creates an OpenCL Native Buffer (GPU or similar) with a host buffer.
     * @param container A container filled with any arbitrary Data, you must make
     *         sure that the alignment corresponds to the Device Buffer.
     * @param type The type of Buffer you want to create can be either READ, WRITE
     *         or READ | WRITE @see buffer_type
     * @param name (optional) The name of the Buffer, this is useful for
     *         automatically entering the buffer into a kernel
     * @return a Native Buffer Object
    */
    template <class T>
    static Buffer createBuffer(std::vector<T>& container,buffer_type type,std::string name = "Unnamed")
    {
        return Buffer(
            m_context,
            reinterpret_cast<byte*>(container.data()), // data as void-ptr
            container.size() * sizeof( T ),  // size with data as void-ptr
            type,
            std::forward<std::string>(name)
        );
    }

private:

    static bool m_initialized;
    static cl_context m_context;
    static cl_platform_id m_platform_id;
    static cl_device_id m_device_id;
};
}
