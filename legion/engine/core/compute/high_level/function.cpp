#include <core/compute/high_level/function.hpp>
#include <core/compute/context.hpp>

#include <inaddr.h>


namespace legion::core::compute
{
    common::result<void, void> function_base::invoke(size_type global, std::vector<std::pair<buffer_base*, buffer_type>>& parameters) const
    {
        std::vector<Buffer> buffers;
        buffers.reserve(parameters.size());

        for (auto& [base, type] : parameters)
        {
            buffers.emplace_back(Context::createBuffer(base->container.first, base->container.second, type, base->name));
        }
        m_kernel->local(512).global(global);

        m_kernel->read_write_mode(buffer_type::READ_BUFFER);

        cl_uint i = 0;
        for (auto& buffer : buffers)
        {
            if (buffer.has_name())
            {
                m_kernel->set_buffer(buffer);
            }
            else {
                m_kernel->set_buffer(buffer, i);
                i++;
            }

            if (buffer.is_readbuffer())
            {
                m_kernel->enqueue_buffer(buffer);
            }

        }

        m_kernel->dispatch();

        m_kernel->read_write_mode(buffer_type::WRITE_BUFFER);

        for (auto& buffer : buffers)
        {
            if (buffer.is_writebuffer())
            {
                m_kernel->enqueue_buffer(buffer);
            }

        }
        m_kernel->finish();

        return common::Ok();
    }
}
