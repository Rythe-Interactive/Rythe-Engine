#include <rendering/data/buffer.hpp>

namespace legion::rendering
{
    void buffer::idDeleter(app::gl_id& value)
    {
        if (!app::ContextHelper::initialized())
            return;

#if defined(LEGION_DEBUG)
        if (!app::ContextHelper::getCurrentContext())
        {
            log::error("No current context to delete buffer with.");
            return;
        }
#endif
        if (value)
            glDeleteBuffers(1, &value);
    }

    buffer::buffer(GLenum target, size_type size, void* data, GLenum usage)
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
        glBufferData(target, size, data, usage); // Allocate VRAM
        glBindBuffer(target, 0);
    }

    buffer::buffer(GLenum target, GLenum usage)
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
        glBindBuffer(target, 0);
    }

    L_NODISCARD app::gl_id buffer::id() const
    {
        return m_id;
    }

    L_NODISCARD GLenum buffer::target() const
    {
        return m_target;
    }

    L_NODISCARD GLenum buffer::usage() const
    {
        return m_usage;
    }

    L_NODISCARD size_type buffer::size() const
    {
#if defined(LEGION_DEBUG)
        if (!app::ContextHelper::getCurrentContext())
        {
            log::error("No current context to read data from.");
            return 0;
        }
#endif

        size_type size;
        glBindBuffer(m_target, m_id);
        glGetBufferParameteri64v(m_target, GL_BUFFER_SIZE, reinterpret_cast<GLint64*>(&size)); // Fetch VRAM size of the currently bound buffer.
        glBindBuffer(m_target, 0);
        return size;
    }

    void buffer::bindBufferBase(uint index) const
    {
#if defined(LEGION_DEBUG)
        if (!app::ContextHelper::getCurrentContext())
        {
            log::error("No current context to work with.");
            return;
        }

        if (m_target != GL_ATOMIC_COUNTER_BUFFER && m_target != GL_TRANSFORM_FEEDBACK_BUFFER && m_target != GL_UNIFORM_BUFFER && m_target != GL_SHADER_STORAGE_BUFFER)
        {
            log::error("Attempt at binding buffer base of an invalid target. Target must be GL_ATOMIC_COUNTER_BUFFER, GL_TRANSFORM_FEEDBACK_BUFFER, GL_UNIFORM_BUFFER or GL_SHADER_STORAGE_BUFFER. id: {}", m_id.value);
            return;
        }
#endif
        glBindBuffer(m_target, m_id);
        glBindBufferBase(m_target, index, m_id); // Bind to indexed buffer location.
        glBindBuffer(m_target, 0);
    }

    void buffer::resize(size_type newSize) const
    {
#if defined(LEGION_DEBUG)
        if (!app::ContextHelper::getCurrentContext())
        {
            log::error("No current context to work with.");
            return;
        }
#endif

        glBindBuffer(m_target, m_id);
        glBufferData(m_target, newSize, nullptr, m_usage); // Reallocate VRAM.
        glBindBuffer(m_target, 0);
    }

    void buffer::bufferData(size_type size, void* data) const
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

        if (oldSize >= size)
            glBufferSubData(m_target, 0, size, data); // If the new data fits within the already allocated VRAM then we don't want to reallocate.
        else
            glBufferData(m_target, size, data, m_usage); // If the new data does not fit within the already allocated VRAM we want to reallocate the buffer. 

        glBindBuffer(m_target, 0);
    }

    void buffer::bufferData(size_type offset, size_type size, void* data) const
    {
#if defined(LEGION_DEBUG)
        if (!app::ContextHelper::getCurrentContext())
        {
            log::error("No current context to work with.");
            return;
        }

        size_type oldSize;
        glBindBuffer(m_target, m_id);
        glGetBufferParameteri64v(m_target, GL_BUFFER_SIZE, reinterpret_cast<GLint64*>(&oldSize)); // Fetch VRAM size of the currently bound buffer.
        glBindBuffer(m_target, 0);
        if (offset + size > oldSize)
        {
            log::error("Offset + size cannot be more that the size of the buffer.");
            return;
        }
#endif
        glBindBuffer(m_target, m_id);
        glBufferSubData(m_target, offset, size, data); // Write data to already allocated VRAM.
        glBindBuffer(m_target, 0);
    }

    void buffer::bind() const
    {
#if defined(LEGION_DEBUG)
        if (!app::ContextHelper::getCurrentContext())
        {
            log::error("No current context to work with.");
            return;
        }
#endif

        glBindBuffer(m_target, m_id);
    }

    void buffer::release() const
    {
#if defined(LEGION_DEBUG)
        if (!app::ContextHelper::getCurrentContext())
        {
            log::error("No current context to work with.");
            return;
        }
#endif

        glBindBuffer(m_target, 0);
    }

}
