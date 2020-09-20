#pragma once
#include <rendering/detail/engine_include.hpp>
#include <rendering/data/texture.hpp>
#include <rendering/data/renderbuffer.h>

namespace args::rendering
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

        void attach(const renderbuffer& renderbuffer, GLenum attachement)
        {
            glNamedFramebufferRenderbuffer(id, attachement, GL_RENDERBUFFER, renderbuffer.id);
        }

        void attach(const )

        void release()
        {
            glBindFramebuffer(target, 0);
        }
    };
}
