#include <rendering/data/renderbuffer.hpp>

namespace legion::rendering
{
    renderbuffer::renderbuffer(GLenum internalformat, math::ivec2 resolution, uint samples)
        : m_id([](app::gl_id& value) { // Assign logic for renderbuffer deletion to managed resource.
#if defined(LEGION_DEBUG)
            if (!app::ContextHelper::getCurrentContext())
            {
                if (app::ContextHelper::initialized())
                    log::error("No current context to delete renderbuffer with.");
                return;
            }
#endif
            if (value)
                glDeleteRenderbuffers(1, &value);
        }, invalid_id),
        m_samples(samples),
        m_internalformat(internalformat)
    {
#if defined(LEGION_DEBUG)
        if (!app::ContextHelper::getCurrentContext())
        {
            log::error("No current context to create renderbuffer with.");
            return;
        }
#endif
        glGenRenderbuffers(1, &m_id); // Generate framebuffer
        glBindRenderbuffer(GL_RENDERBUFFER, m_id);
        if (samples > 0)
            glNamedRenderbufferStorageMultisample(m_id, samples, internalformat, resolution.x, resolution.y); // Allocate multi-sampling capable VRAM storage
        else
            glNamedRenderbufferStorage(m_id, internalformat, resolution.x, resolution.y); // Allocate VRAM
        glBindRenderbuffer(GL_RENDERBUFFER, 0);
    }

    renderbuffer::renderbuffer(GLenum internalformat, int width, int height, uint samples)
        : m_id([](app::gl_id& value) { // Assign logic for renderbuffer deletion to managed resource.
#if defined(LEGION_DEBUG)
            if (!app::ContextHelper::getCurrentContext())
            {
                if (app::ContextHelper::initialized())
                    log::error("No current context to delete renderbuffer with.");
                return;
            }
#endif
            if (value)
                glDeleteRenderbuffers(1, &value);
        }, invalid_id),
        m_samples(samples),
        m_internalformat(internalformat)
    {
#if defined(LEGION_DEBUG)
        if (!app::ContextHelper::getCurrentContext())
        {
            log::error("No current context to create renderbuffer with.");
            return;
        }
#endif
        glGenRenderbuffers(1, &m_id); // Generate framebuffer
        glBindRenderbuffer(GL_RENDERBUFFER, m_id);
        if (samples > 0)
            glNamedRenderbufferStorageMultisample(m_id, samples, internalformat, width, height); // Allocate multi-sampling capable VRAM storage
        else
            glNamedRenderbufferStorage(m_id, internalformat, width, height); // Allocate VRAM
        glBindRenderbuffer(GL_RENDERBUFFER, 0);
    }

    renderbuffer::renderbuffer(GLenum internalformat, uint samples)
        : m_id([](app::gl_id& value) { // Assign logic for renderbuffer deletion to managed resource.
#if defined(LEGION_DEBUG)
            if (!app::ContextHelper::getCurrentContext())
            {
                if (app::ContextHelper::initialized())
                    log::error("No current context to delete renderbuffer with.");
                return;
            }
#endif
            if (value)
                glDeleteRenderbuffers(1, &value);
        }, invalid_id),
        m_samples(samples),
        m_internalformat(internalformat)
    {
#if defined(LEGION_DEBUG)
        if (!app::ContextHelper::getCurrentContext())
        {
            log::error("No current context to create renderbuffer with.");
            return;
        }
#endif
        glGenRenderbuffers(1, &m_id); // Generate framebuffer
    }

    void renderbuffer::bind() const
    {
#if defined(LEGION_DEBUG)
        if (!app::ContextHelper::getCurrentContext())
        {
            log::error("No current context to work with.");
            return;
        }
#endif
        glBindRenderbuffer(GL_RENDERBUFFER, m_id);
    }

    void renderbuffer::release()
    {
#if defined(LEGION_DEBUG)
        if (!app::ContextHelper::getCurrentContext())
        {
            log::error("No current context to work with.");
            return;
        }
#endif
        glBindRenderbuffer(GL_RENDERBUFFER, 0);
    }

    L_NODISCARD app::gl_id renderbuffer::id() const
    {
        return m_id;
    }

    L_NODISCARD size_type renderbuffer::samples() const
    {
        return m_samples;
    }

    L_NODISCARD GLenum renderbuffer::format() const
    {
        return m_internalformat;
    }

    L_NODISCARD math::ivec2 renderbuffer::size() const
    {
#if defined(LEGION_DEBUG)
        if (!app::ContextHelper::getCurrentContext())
        {
            log::error("No current context to read from.");
            return math::ivec2(0, 0);
        }
#endif
        math::ivec2 size;
        glBindRenderbuffer(GL_RENDERBUFFER, m_id);
        glGetRenderbufferParameteriv(GL_RENDERBUFFER, GL_RENDERBUFFER_WIDTH, &size.x); // Fetch width.
        glGetRenderbufferParameteriv(GL_RENDERBUFFER, GL_RENDERBUFFER_HEIGHT, &size.y); // Fetch height.
        glBindRenderbuffer(GL_RENDERBUFFER, 0);
        return size;
    }

    void renderbuffer::resize(math::ivec2 newSize) const
    {
#if defined(LEGION_DEBUG)
        if (!app::ContextHelper::getCurrentContext())
        {
            log::error("No current context to work with.");
            return;
        }
#endif
        glBindRenderbuffer(GL_RENDERBUFFER, m_id);
        if (m_samples > 0)
            glNamedRenderbufferStorageMultisample(m_id, m_samples, m_internalformat, newSize.x, newSize.y); // Allocate multi-sampling capable VRAM storage
        else
            glNamedRenderbufferStorage(m_id, m_internalformat, newSize.x, newSize.y); // Allocate VRAM
        glBindRenderbuffer(GL_RENDERBUFFER, 0);
    }

}
