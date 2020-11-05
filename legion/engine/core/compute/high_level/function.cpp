#include <core/compute/high_level/function.hpp>
#include <core/compute/context.hpp>


namespace legion::core::compute
{
    common::result<void, void> function_base::invoke(size_type global, invoke_buffer_container& parameters) const
    {
        std::vector<Buffer> buffers;
        buffers.reserve(parameters.size());

        for (auto& [base, type] : parameters)
        {
            buffers.emplace_back(Context::createBuffer(base->container.first, base->container.second, type, base->name));
        }
        return invoke2(global,buffers);
    }

    common::result<void, void> function_base::invoke2(size_type global, std::vector<Buffer> buffers) const
    {
        m_kernel->local(m_locals).global(global);

        m_kernel->readWriteMode(buffer_type::READ_BUFFER);

        cl_uint i = 0;
        for (auto& buffer : buffers)
        {
            if (buffer.hasName())
            {
                m_kernel->setBuffer(buffer);
            }
            else {
                m_kernel->setBuffer(buffer, i);
                i++;
            }

            if (buffer.isReadBuffer())
            {
                m_kernel->enqueueBuffer(buffer);
            }

        }

        m_kernel->dispatch();

        m_kernel->readWriteMode(buffer_type::WRITE_BUFFER);

        for (const auto& buffer : buffers)
        {
            if (buffer.isWriteBuffer())
            {
                m_kernel->enqueueBuffer(buffer);
            }

        }
        m_kernel->finish();

        return common::Ok();
    }
}
