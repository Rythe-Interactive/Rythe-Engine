#pragma once
#include <CL/cl.h>

#include <core/filesystem/resource.hpp>
#include <core/compute/kernel.hpp>

#include <functional>
#include <string>

/**
 * @file program.hpp
 */

namespace args::core::compute {

    /** @class Program
     *  @brief A Mid-Level Wrapper around a cl_program, creates command-queues
     *          and Kernels for you automatically
     */
    class ARGS_API Program
    {
    public:


        Program(cl_context, cl_device_id, filesystem::basic_resource);
        Program(const Program& other) = default;
        Program(Program&& other) noexcept = default;
        Program& operator=(const Program& other) = default;
        Program& operator=(Program&& other) noexcept = default;

        /**
         * @brief Creates a wrapped Kernel
         * @param name The name of the kernel you want to load/get
         * @return A Kernel Object
        */
        Kernel kernelContext(const std::string& name);

        /**
         * @brief Makes sure that a kernel is loaded, otherwise the kernel
         *         will be lazy initialized on first request
         *
         * @param name The name of the Kernel you want to load (function-name)
         * @return A raw cl_kernel object
        */
        cl_kernel prewarm(const std::string& name);


        /**
         * @brief Creates a Command Queue
         */
        cl_command_queue make_cq() const
        {
            return make_command_queue();
        }

        static void from_resource(Program* value, const filesystem::basic_resource& resource);

    private:
        friend filesystem::basic_resource;
        template <class T, class C1,class C2,class C3>
        friend T filesystem::from_resource(const filesystem::basic_resource& resource);

        Program() = default;

        std::function<cl_command_queue()> make_command_queue;
        cl_program m_program;
        std::unordered_map<std::string, cl_kernel> m_kernelCache;
    };
}
