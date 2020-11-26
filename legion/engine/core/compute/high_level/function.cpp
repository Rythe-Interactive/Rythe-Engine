#include <core/compute/high_level/function.hpp>
#include <core/compute/context.hpp>


namespace legion::core::compute
{
    common::result<void, void> function_base::invoke(dvar global, invoke_buffer_container& parameters) const
    {
        std::vector<Buffer> buffers;
        buffers.reserve(parameters.size());

        for (auto& [base, type] : parameters)
        {
            buffers.emplace_back(Context::createBuffer(base->container.first, base->container.second, type, base->name));
        }
        return invoke2(std::move(global),buffers);
    }

    common::result<void, void> function_base::invoke2(dvar global, std::vector<Buffer> buffers) const
    {

        if(std::holds_alternative<std::tuple<size_type,size_type,size_type>>(global))
        {
            auto& [s0,s1,s2] = std::get<2>(global);
            m_kernel->local(m_locals).global(s0,s1,s2);
        }

        if(std::holds_alternative<std::tuple<size_type,size_type>>(global))
        {
            auto& [s0,s1] = std::get<1>(global);
            m_kernel->local(m_locals).global(s0,s1);
        }
        if(std::holds_alternative<std::tuple<size_type>>(global))
        {
            auto& [s0] = std::get<0>(global);
            m_kernel->local(m_locals).global(s0);
        }

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
