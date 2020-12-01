#pragma once
#include <application/application.hpp>
#include <rendering/data/texture.hpp>
#include <rendering/data/renderbuffer.hpp>

namespace legion::rendering
{
    struct framebuffer
    {
        //Id of the framebuffer.
        app::gl_id id;
        //type of framebuffer, either read, write or both.
        GLenum target;

        framebuffer(GLenum target = GL_FRAMEBUFFER) : target(target)
        {
            glGenFramebuffers(1, &id);
        }

        ~framebuffer()
        {
            glDeleteFramebuffers(1, &id);
        }

        bool verify()
        {
            return glCheckFramebufferStatus(target) == GL_FRAMEBUFFER_COMPLETE;
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
            glNamedFramebufferTexture(id, attachment, GL_TEXTURE_2D, texture.get_texture().textureId);
        }

        void release()
        {
            glBindFramebuffer(target, 0);
        }
    };
}
