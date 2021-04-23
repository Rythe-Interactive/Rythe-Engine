#include "buffer.hpp"
#pragma once

namespace legion::rendering
{
    template<typename T>
    buffer::buffer(GLenum target, const std::vector<T>& data, GLenum usage)
        : m_id(&buffer::idDeleter, invalid_id),
        m_target(target),
        m_usage(usage)
    {
#if defined(LEGION_DEBUG)
        if (!app::ContextHelper::getCurrentContext())
        {
            log::error("No current context to create buffer with.");
            return;
        }
#endif
        glGenBuffers(1, &m_id); // Generate buffer
        glBindBuffer(target, m_id); // First bind tells OpenGL what kind of buffer it will be.
        glBufferData(target, sizeof(T) * data.size(), data.data(), usage); // Allocate VRAM
        glBindBuffer(target, 0);
    }

    template<typename T>
    inline void buffer::bufferData(const std::vector<T>& data) const
    {
#if defined(LEGION_DEBUG)
        if (!app::ContextHelper::getCurrentContext())
        {
            log::error("No current context to work with.");
            return;
        }
#endif

        glBindBuffer(m_target, m_id);

        size_type oldSize;
        glGetBufferParameteri64v(m_target, GL_BUFFER_SIZE, reinterpret_cast<GLint64*>(&oldSize)); // Fetch the previous size of the buffer.

        auto dataSize = data.size() * sizeof(T);
        if (oldSize >= dataSize)
            glBufferSubData(m_target, 0, dataSize, data.data()); // If the new data fits within the already allocated VRAM then we don't want to reallocate.
        else
            glBufferData(m_target, dataSize, data.data(), m_usage); // If the new data does not fit within the already allocated VRAM we want to reallocate the buffer. 

        glBindBuffer(m_target, 0);
    }

}
