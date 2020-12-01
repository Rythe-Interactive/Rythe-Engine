#pragma once
#include <application/application.hpp>
#include <rendering/data/texture.hpp>
#include <rendering/data/renderbuffer.hpp>

namespace legion::rendering
{
    struct framebuffer
    {
        app::gl_id id;
        GLenum target;

        framebuffer(GLenum target) : target(target)
        {
            glGenFramebuffers(1, &id);
        }

        ~framebuffer()
        {
            glDeleteFramebuffers(1, &id);
        }

        void bind()
        {
            glBindFramebuffer(target, id);
        }

        void attach(const renderbuffer& renderbuffer, GLenum attachment)
        {
            glNamedFramebufferRenderbuffer(id, attachment, GL_RENDERBUFFER, renderbuffer.id);
        }

        void attach(texture_handle texture, GLenum attachment)
        {
            glNamedFramebufferTexture(id, attachment, GL_RENDERBUFFER, texture.get_texture().textureId);
        }

        void release()
        {
            glBindFramebuffer(target, 0);
        }
    };
}
