#pragma once
#include <application/application.hpp>

namespace legion::rendering
{
    struct renderbuffer
    {
    private:
        common::managed_resource<app::gl_id> m_id = common::managed_resource<app::gl_id>(nullptr);
        uint m_samples = 0;
    public:
        renderbuffer() = default;

        renderbuffer(GLenum internalformat, math::ivec2 resolution, uint samples = 0) : m_id([](app::gl_id& value) { glDeleteRenderbuffers(1, &value); }, invalid_id), m_samples(samples)
        {
            glGenRenderbuffers(1, &m_id);
            glBindRenderbuffer(GL_RENDERBUFFER, m_id);
            if (samples > 0)
                glNamedRenderbufferStorageMultisample(m_id, samples, internalformat, resolution.x, resolution.y);
            else
                glNamedRenderbufferStorage(m_id, internalformat, resolution.x, resolution.y);
            glBindRenderbuffer(GL_RENDERBUFFER, 0);
        }

        renderbuffer(GLenum internalformat, int width, int height, uint samples = 0) : m_id([](app::gl_id& value) { glDeleteRenderbuffers(1, &value); }, invalid_id), m_samples(samples)
        {
            glGenRenderbuffers(1, &m_id);
            glBindRenderbuffer(GL_RENDERBUFFER, m_id);
            if (samples > 0)
                glNamedRenderbufferStorageMultisample(m_id, samples, internalformat, width, height);
            else
                glNamedRenderbufferStorage(m_id, internalformat, width, height);
            glBindRenderbuffer(GL_RENDERBUFFER, 0);
        }

        void bind() const
        {
            glBindRenderbuffer(GL_RENDERBUFFER, m_id);
        }

        static void release()
        {
            glBindRenderbuffer(GL_RENDERBUFFER, 0);
        }

        L_NODISCARD app::gl_id id() const
        {
            return m_id;
        }

        L_NODISCARD size_type samples() const
        {
            return m_samples;
        }

        L_NODISCARD GLenum format() const
        {
            GLenum internalformat = 0;
            glBindRenderbuffer(GL_RENDERBUFFER, m_id);
            glGetRenderbufferParameteriv(GL_RENDERBUFFER, GL_RENDERBUFFER_INTERNAL_FORMAT, reinterpret_cast<GLint*>(&internalformat));
            glBindRenderbuffer(GL_RENDERBUFFER, 0);
            return internalformat;
        }

        L_NODISCARD math::ivec2 size() const
        {
            math::ivec2 size;
            glBindRenderbuffer(GL_RENDERBUFFER, m_id);
            glGetRenderbufferParameteriv(GL_RENDERBUFFER, GL_RENDERBUFFER_WIDTH, &size.x);
            glGetRenderbufferParameteriv(GL_RENDERBUFFER, GL_RENDERBUFFER_HEIGHT, &size.y);
            glBindRenderbuffer(GL_RENDERBUFFER, 0);
            return size;
        }

        void resize(math::ivec2 newSize) const
        {
            glBindRenderbuffer(GL_RENDERBUFFER, m_id);
            GLenum internalformat;
            glGetRenderbufferParameteriv(GL_RENDERBUFFER, GL_RENDERBUFFER_INTERNAL_FORMAT, reinterpret_cast<GLint*>(&internalformat));
            if (m_samples > 0)
                glNamedRenderbufferStorageMultisample(m_id, m_samples, internalformat, newSize.x, newSize.y);
            else
                glNamedRenderbufferStorage(m_id, internalformat, newSize.x, newSize.y);
            glBindRenderbuffer(GL_RENDERBUFFER, 0);
        }

    };
}
