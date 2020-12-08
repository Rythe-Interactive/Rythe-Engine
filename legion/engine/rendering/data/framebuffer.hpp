#pragma once
#include <application/application.hpp>
#include <rendering/data/texture.hpp>
#include <rendering/data/renderbuffer.hpp>
#include <any>
#include <unordered_map>

namespace legion::rendering
{
    struct framebuffer
    {
    private:
        //Id of the framebuffer.
        common::managed_resource<app::gl_id> m_id = common::managed_resource<app::gl_id>(nullptr);
        //type of framebuffer, either read, write or both.
        GLenum target = GL_FRAMEBUFFER;

        std::unordered_map<GLenum, std::any> attachments;

    public:
        framebuffer() = default;

        explicit framebuffer(GLenum target) : m_id([](app::gl_id& value) { glDeleteFramebuffers(1, &value); }, invalid_id), target(target)
        {
            glGenFramebuffers(1, &m_id);
        }

        std::pair<bool, std::string> verify() const
        {
            const auto verification = glCheckNamedFramebufferStatus(m_id, target);
            auto result = std::make_pair(false, "Unkown framebuffer error: " + std::to_string(verification));
            switch (verification)
            {
            case GL_FRAMEBUFFER_COMPLETE:
                result = { true, "Framebuffer is complete." };
                return result;
            case GL_FRAMEBUFFER_UNDEFINED:
                result = { false, "Framebuffer set to default but default does not exist." };
                return result;
            case GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT:
                result = { false, "One or more attachments of the framebuffer are incomplete." };
                return result;
            case GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT:
                result = { false, "Framebuffer does not have atleast one image attached to it." };
                return result;
            case GL_FRAMEBUFFER_INCOMPLETE_DRAW_BUFFER:
                result = { false, "Value of GL_FRAMEBUFFER_ATTACHMENT_OBJECT_TYPE is GL_NONE for any color attachment point(s) named by GL_DRAW_BUFFERi" };
                return result;
            case GL_FRAMEBUFFER_INCOMPLETE_READ_BUFFER:
                result = { false, "GL_READ_BUFFER is not GL_NONE and the value of GL_FRAMEBUFFER_ATTACHMENT_OBJECT_TYPE is GL_NONE for the color attachment point named by GL_READ_BUFFER" };
                return result;
            case GL_FRAMEBUFFER_UNSUPPORTED:
                result = { false, "The combination of internal formats of the attached images violates an implementation-dependent set of restrictions" };
                return result;
            case GL_FRAMEBUFFER_INCOMPLETE_MULTISAMPLE:
                result = { false, "The value of GL_RENDERBUFFER_SAMPLES is not the same for all attached renderbuffers; if the value of GL_TEXTURE_SAMPLES is the not same for all attached textures; If the attached images are a mix of renderbuffers and textures, the value of GL_RENDERBUFFER_SAMPLES does not match the value of GL_TEXTURE_SAMPLES; or, the value of GL_TEXTURE_FIXED_SAMPLE_LOCATIONS is not the same for all attached textures; or, if the attached images are a mix of renderbuffers and textures, the value of GL_TEXTURE_FIXED_SAMPLE_LOCATIONS is not GL_TRUE for all attached textures." };
                return result;
            case GL_FRAMEBUFFER_INCOMPLETE_LAYER_TARGETS:
                result = { false, "If any framebuffer attachment is layered, and any populated attachment is not layered, or if all populated color attachments are not from textures of the same target." };
                return result;
            default:
                return result;
            }
        }

        void bind() const
        {
            glBindFramebuffer(target, m_id);
        }

        void attach(renderbuffer rbo, GLenum attachment)
        {
            glNamedFramebufferRenderbuffer(m_id, attachment, GL_RENDERBUFFER, rbo.id());
            attachments[attachment] = std::make_any<renderbuffer>(rbo);
        }

        void attach(texture_handle texture, GLenum attachment)
        {
            glNamedFramebufferTexture(m_id, attachment, texture.get_texture().textureId, 0);
            attachments[attachment] = std::make_any<texture_handle>(texture);
        }

        L_NODISCARD const std::any& getAttachment(GLenum attachment) const
        {
            if (!attachments.count(attachment))
                return std::any();
            return attachments.at(attachment);
        }

        void release() const
        {
            glBindFramebuffer(target, 0);
        }

        L_NODISCARD operator bool() const
        {
            return m_id;
        }
    };
}
