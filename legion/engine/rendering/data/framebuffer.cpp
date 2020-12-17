#include <rendering/data/framebuffer.hpp>

namespace legion::rendering
{
    framebuffer::framebuffer(GLenum target)
        : m_id([](app::gl_id& value) { // Assign logic for framebuffer deletion to managed resource.
            if (!app::ContextHelper::initialized())
                return;

#if defined(LEGION_DEBUG)
        if (!app::ContextHelper::getCurrentContext())
        {
            log::error("No current context to delete framebuffer with.");
            return;
        }
#endif
        if (value)
            glDeleteFramebuffers(1, &value);
            }, invalid_id),
        m_target(target)
    {
#if defined(LEGION_DEBUG)
        if (!app::ContextHelper::getCurrentContext())
        {
            log::error("No current context to create framebuffer with.");
            return;
        }
#endif
        glGenFramebuffers(1, &m_id); // Generate framebuffer
    }

    std::pair<bool, std::string> framebuffer::verify() const
    {
#if defined(LEGION_DEBUG)
        if (!app::ContextHelper::getCurrentContext())
        {
            log::error("No current context to work with.");
            return { false, "No context made current." };
        }
#endif
        // Check if the framebuffer was ever even generated.
        if (!m_id.value)
            return { false, "Framebuffer has not been generated yet." };

        glBindFramebuffer(m_target, m_id);
        const auto verification = glCheckNamedFramebufferStatus(m_id, m_target); // Fetch the framebuffer status.
        glBindFramebuffer(m_target, 0);

#if defined(LEGION_DEBUG)
        // Initialize to success so no extra allocations need to happen when everything is good.
        std::pair<bool, std::string> result(true, std::string("Framebuffer is complete."));

        // Check the verification status.
        switch (verification)
        {
        case GL_FRAMEBUFFER_COMPLETE: // Leave the preinitialized values be.
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

        if (!result.first) // If the framebuffer isn't complete we might want to know what the state of the framebuffer is, so we print the attachments.
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
#else
        // In release we skip all checks and only check if it's actually valid. For more debug information the engine should be ran in debug mode.
        if (verification == GL_FRAMEBUFFER_COMPLETE)
            return { true, std::string("Framebuffer is complete.") };
        return { false, std::string("Framebuffer isn't complete.") };
#endif
    }

    GLenum framebuffer::target() const
    {
        return m_target;
    }

    app::gl_id framebuffer::id() const
    {
        return m_id;
    }

    void framebuffer::bind() const
    {
#if defined(LEGION_DEBUG)
        if (!app::ContextHelper::getCurrentContext())
        {
            log::error("No current context to work with.");
            return;
        }
#endif

        glBindFramebuffer(m_target, m_id);
    }

    void framebuffer::attach(renderbuffer rbo, GLenum attachment)
    {
#if defined(LEGION_DEBUG)
        if (!app::ContextHelper::getCurrentContext())
        {
            log::error("No current context to work with.");
            return;
        }
#endif
        if (m_id.value == 0)
        {
            log::error("Attempting to attach render targets to default framebuffer.");
            return;
        }

        glBindFramebuffer(m_target, m_id);
        glNamedFramebufferRenderbuffer(m_id, attachment, GL_RENDERBUFFER, rbo.id()); // Attach the renderbuffer.
        glBindFramebuffer(m_target, 0);
        m_attachments[attachment] = rbo; // Insert the renderbuffer into the map of attachments.
    }

    void framebuffer::attach(texture_handle texture, GLenum attachment)
    {
#if defined(LEGION_DEBUG)
        if (!app::ContextHelper::getCurrentContext())
        {
            log::error("No current context to work with.");
            return;
        }
#endif
        if (m_id.value == 0)
        {
            log::error("Attempting to attach render targets to default framebuffer.");
            return;
        }

        glBindFramebuffer(m_target, m_id);
        glNamedFramebufferTexture(m_id, attachment, texture.get_texture().textureId, 0); // Attach the texture.
        glBindFramebuffer(m_target, 0);
        m_attachments[attachment] = texture; // Insert the texture into the map of attachments.
    }

    void framebuffer::detach(GLenum attachment)
    {
        if (m_id.value == 0)
        {
            log::error("Attempting to detach render targets to default framebuffer.");
            return;
        }

        if (!m_attachments.count(attachment))
            return;

        auto att = m_attachments.at(attachment); // Otherwise return the active attachment.

        if (std::holds_alternative<std::monostate>(att))
            return;

        glBindFramebuffer(m_target, m_id);
        if (std::holds_alternative<texture_handle>(att))
            glNamedFramebufferTexture(m_id, attachment, 0, 0); // Attach the texture.
        else
            glNamedFramebufferRenderbuffer(m_id, attachment, GL_RENDERBUFFER, 0); // Attach the renderbuffer.

        glBindFramebuffer(m_target, 0);

        m_attachments.erase(attachment);
    }

    void framebuffer::attach(attachment att, GLenum attachment)
    {
#if defined(LEGION_DEBUG)
        if (!app::ContextHelper::getCurrentContext())
        {
            log::error("No current context to work with.");
            return;
        }

        if (m_id.value == 0)
        {
            log::error("Attempting to attach render targets to default framebuffer.");
            return;
        }

        if(std::holds_alternative<std::monostate>(att))
        {
            log::error("Attempting to attach empty attachment to framebuffer.");
            return;
        }
#endif

        glBindFramebuffer(m_target, m_id);
        if (std::holds_alternative<texture_handle>(att))
            glNamedFramebufferTexture(m_id, attachment, std::get<texture_handle>(att).get_texture().textureId, 0); // Attach the texture.
        else
            glNamedFramebufferRenderbuffer(m_id, attachment, GL_RENDERBUFFER, std::get<renderbuffer>(att).id()); // Attach the renderbuffer.

        glBindFramebuffer(m_target, 0);
        m_attachments[attachment] = att; // Insert the attachment into the map of attachments.
    }

    L_NODISCARD const attachment& framebuffer::getAttachment(GLenum attachment) const
    {
        if (m_id.value == 0)
        {
            log::error("Attempting to attach render targets to default framebuffer.");
            return invalid_attachment;
        }

        if (!m_attachments.count(attachment)) // Return an empty std::any if the attachment wasn't active.
            return invalid_attachment;
        return m_attachments.at(attachment); // Otherwise return the active attachment.
    }

    void framebuffer::release() const
    {
#if defined(LEGION_DEBUG)
        if (!app::ContextHelper::getCurrentContext())
        {
            log::error("No current context to work with.");
            return;
        }
#endif

        glBindFramebuffer(m_target, 0);
    }

    L_NODISCARD framebuffer::operator bool() const
    {
        return m_id.value;
    }
}
