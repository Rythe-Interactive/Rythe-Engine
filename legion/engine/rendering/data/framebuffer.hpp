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
        GLenum m_target = GL_FRAMEBUFFER;

        std::unordered_map<GLenum, std::any> m_attachments;

    public:
        framebuffer() = default;

        explicit framebuffer(GLenum target) : m_id([](app::gl_id& value) { glDeleteFramebuffers(1, &value); }, invalid_id), m_target(target)
        {
            glGenFramebuffers(1, &m_id);
        }

        std::pair<bool, std::string> verify() const
        {
            glBindFramebuffer(m_target, m_id);
            const auto verification = glCheckNamedFramebufferStatus(m_id, m_target);
            glBindFramebuffer(m_target, 0);
            std::pair<bool, std::string> result(true, std::string("Framebuffer is complete."));
            switch (verification)
            {
            case GL_FRAMEBUFFER_COMPLETE:
            break;
            case GL_FRAMEBUFFER_UNDEFINED:
                result = { false, "Id: " + std::to_string(m_id.value) + " Framebuffer set to default but default does not exist." };
                break;
            case GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT:
                result = { false, "Id: " + std::to_string(m_id.value) + " One or more attachments of the framebuffer are incomplete." };
                break;
            case GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT:
                result = { false, "Id: " + std::to_string(m_id.value) + " Framebuffer does not have atleast one image attached to it." };
                break;
            case GL_FRAMEBUFFER_INCOMPLETE_DRAW_BUFFER:
                result = { false, "Id: " + std::to_string(m_id.value) + " Value of GL_FRAMEBUFFER_ATTACHMENT_OBJECT_TYPE is GL_NONE for any color attachment point(s) named by GL_DRAW_BUFFERi" };
                break;
            case GL_FRAMEBUFFER_INCOMPLETE_READ_BUFFER:
                result = { false, "Id: " + std::to_string(m_id.value) + " GL_READ_BUFFER is not GL_NONE and the value of GL_FRAMEBUFFER_ATTACHMENT_OBJECT_TYPE is GL_NONE for the color attachment point named by GL_READ_BUFFER" };
                break;
            case GL_FRAMEBUFFER_UNSUPPORTED:
                result = { false, "Id: " + std::to_string(m_id.value) + " The combination of internal formats of the attached images violates an implementation-dependent set of restrictions" };
                break;
            case GL_FRAMEBUFFER_INCOMPLETE_MULTISAMPLE:
                result = { false, "Id: " + std::to_string(m_id.value) + " The value of GL_RENDERBUFFER_SAMPLES is not the same for all attached renderbuffers; if the value of GL_TEXTURE_SAMPLES is the not same for all attached textures; If the attached images are a mix of renderbuffers and textures, the value of GL_RENDERBUFFER_SAMPLES does not match the value of GL_TEXTURE_SAMPLES; or, the value of GL_TEXTURE_FIXED_SAMPLE_LOCATIONS is not the same for all attached textures; or, if the attached images are a mix of renderbuffers and textures, the value of GL_TEXTURE_FIXED_SAMPLE_LOCATIONS is not GL_TRUE for all attached textures." };
                break;
            case GL_FRAMEBUFFER_INCOMPLETE_LAYER_TARGETS:
                result = { false, "Id: " + std::to_string(m_id.value) + " If any framebuffer attachment is layered, and any populated attachment is not layered, or if all populated color attachments are not from textures of the same target." };
                break;
            default:
                result = { false, "Id: " + std::to_string(m_id.value) + " Unkown framebuffer error: " + std::to_string(verification) };
                break;
            }

            if (!result.first)
            {
                glBindFramebuffer(m_target, m_id);
                GLint value;
                glGetFramebufferAttachmentParameteriv(m_target, GL_COLOR_ATTACHMENT0, GL_FRAMEBUFFER_ATTACHMENT_OBJECT_TYPE, &value);
                switch (value)
                {
                case GL_NONE:
                    log::error("Color attachment is of type GL_NONE");
                    break;
                case GL_FRAMEBUFFER_DEFAULT:
                    log::error("Color attachment is of type GL_FRAMEBUFFER_DEFAULT");
                    break;
                case GL_TEXTURE:
                    log::error("Color attachment is of type GL_TEXTURE");
                    break;
                case GL_RENDERBUFFER:
                    log::error("Color attachment is of type GL_RENDERBUFFER");
                    break;
                default:
                    log::error("Color attachment is of unknown type: {}", value);
                    break;
                }
                glGetFramebufferAttachmentParameteriv(m_target, GL_DEPTH_ATTACHMENT, GL_FRAMEBUFFER_ATTACHMENT_OBJECT_TYPE, &value);
                switch (value)
                {
                case GL_NONE:
                    log::error("Depth attachment is of type GL_NONE");
                    break;
                case GL_FRAMEBUFFER_DEFAULT:
                    log::error("Depth attachment is of type GL_FRAMEBUFFER_DEFAULT");
                    break;
                case GL_TEXTURE:
                    log::error("Depth attachment is of type GL_TEXTURE");
                    break;
                case GL_RENDERBUFFER:
                    log::error("Depth attachment is of type GL_RENDERBUFFER");
                    break;
                default:
                    log::error("Depth attachment is of unknown type: {}", value);
                    break;
                }
                glGetFramebufferAttachmentParameteriv(m_target, GL_STENCIL_ATTACHMENT, GL_FRAMEBUFFER_ATTACHMENT_OBJECT_TYPE, &value);
                switch (value)
                {
                case GL_NONE:
                    log::error("Stencil attachment is of type GL_NONE");
                    break;
                case GL_FRAMEBUFFER_DEFAULT:
                    log::error("Stencil attachment is of type GL_FRAMEBUFFER_DEFAULT");
                    break;
                case GL_TEXTURE:
                    log::error("Stencil attachment is of type GL_TEXTURE");
                    break;
                case GL_RENDERBUFFER:
                    log::error("Stencil attachment is of type GL_RENDERBUFFER");
                    break;
                default:
                    log::error("Stencil attachment is of unknown type: {}", value);
                    break;
                }
                glBindFramebuffer(m_target, 0);
            }

            return result;
        }

        GLenum target() const
        {
            return m_target;
        }

        app::gl_id id() const
        {
            return m_id;
        }

        void bind() const
        {
            glBindFramebuffer(m_target, m_id);
        }

        void attach(renderbuffer rbo, GLenum attachment)
        {
            if (m_id.value == 0)
            {
                log::error("Attempting to attach render targets to default framebuffer.");
                return;
            }

            glBindFramebuffer(m_target, m_id);
            glNamedFramebufferRenderbuffer(m_id, attachment, GL_RENDERBUFFER, rbo.id());
#if defined(LEGION_DEBUG)
            GLint value;
            glGetFramebufferAttachmentParameteriv(m_target, attachment, GL_FRAMEBUFFER_ATTACHMENT_OBJECT_TYPE, &value);

            std::string attachmentName;
            switch (attachment)
            {
            case GL_COLOR_ATTACHMENT0:
                attachmentName = "Color";
                break;
            case GL_DEPTH_ATTACHMENT:
                attachmentName = "Depth";
                break;
            case GL_STENCIL_ATTACHMENT:
                attachmentName = "Stencil";
                break;
            case GL_DEPTH_STENCIL_ATTACHMENT:
                attachmentName = "Depth-stencil";
                break;
            default:
                attachmentName = "Unknown";
                break;
            }

            switch (value)
            {
            case GL_NONE:
                log::debug("{} attachment is of type GL_NONE", attachmentName);
                break;
            case GL_FRAMEBUFFER_DEFAULT:
                log::debug("{} attachment is of type GL_FRAMEBUFFER_DEFAULT", attachmentName);
                break;
            case GL_TEXTURE:
                log::debug("{} attachment is of type GL_TEXTURE", attachmentName);
                break;
            case GL_RENDERBUFFER:
                log::debug("{} attachment is of type GL_RENDERBUFFER", attachmentName);
                break;
            default:
                log::debug("{} attachment is of unknown type: {}", attachmentName, value);
                break;
            }
#endif
            glBindFramebuffer(m_target, 0);
            m_attachments[attachment] = std::make_any<renderbuffer>(rbo);
        }

        void attach(texture_handle texture, GLenum attachment)
        {
            if (m_id.value == 0)
            {
                log::error("Attempting to attach render targets to default framebuffer.");
                return;
            }

            glBindFramebuffer(m_target, m_id);
            glNamedFramebufferTexture(m_id, attachment, texture.get_texture().textureId, 0);
#if defined(LEGION_DEBUG)
            GLint value;
            glGetFramebufferAttachmentParameteriv(m_target, attachment, GL_FRAMEBUFFER_ATTACHMENT_OBJECT_TYPE, &value);

            std::string attachmentName;
            switch (attachment)
            {
            case GL_COLOR_ATTACHMENT0:
                attachmentName = "Color";
                break;
            case GL_DEPTH_ATTACHMENT:
                attachmentName = "Depth";
                break;
            case GL_STENCIL_ATTACHMENT:
                attachmentName = "Stencil";
                break;
            case GL_DEPTH_STENCIL_ATTACHMENT:
                attachmentName = "Depth-stencil";
                break;
            default:
                attachmentName = "Unknown";
                break;
            }

            switch (value)
            {
            case GL_NONE:
                log::debug("{} attachment is of type GL_NONE", attachmentName);
                break;
            case GL_FRAMEBUFFER_DEFAULT:
                log::debug("{} attachment is of type GL_FRAMEBUFFER_DEFAULT", attachmentName);
                break;
            case GL_TEXTURE:
                log::debug("{} attachment is of type GL_TEXTURE", attachmentName);
                break;
            case GL_RENDERBUFFER:
                log::debug("{} attachment is of type GL_RENDERBUFFER", attachmentName);
                break;
            default:
                log::debug("{} attachment is of unknown type: {}", attachmentName, value);
                break;
            }
#endif
            glBindFramebuffer(m_target, 0);
            m_attachments[attachment] = std::make_any<texture_handle>(texture);
        }

        L_NODISCARD const std::any& getAttachment(GLenum attachment) const
        {
            if (m_id.value == 0)
            {
                log::error("Attempting to attach render targets to default framebuffer.");
                return;
            }

            if (!m_attachments.count(attachment))
                return std::any();
            return m_attachments.at(attachment);
        }

        void release() const
        {
            glBindFramebuffer(m_target, 0);
        }

        L_NODISCARD operator bool() const
        {
            return m_id;
        }
    };
}
