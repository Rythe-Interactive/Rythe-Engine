#pragma once
#include <functional>
#include <CL/cl.h>

#include <core/filesystem/resource.hpp>
#include <string>

#include <core/compute/kernel.hpp>

namespace args::core::compute {


    class ARGS_API Program
    {
    public:


        Program(cl_context, cl_device_id, filesystem::basic_resource);


        Program(const Program& other) = default;
        Program(Program&& other) noexcept = default;
        Program& operator=(const Program& other) = default;
        Program& operator=(Program&& other) noexcept = default;

        Kernel kernelContext(const std::string& name);

        cl_kernel prewarm(const std::string& name);

        cl_command_queue make_cq() const
        {
            return make_command_queue();
        }

    private:

        std::function<cl_command_queue()> make_command_queue;
        cl_program m_program;
        std::unordered_map<std::string, cl_kernel> m_kernelCache;
    };
}
