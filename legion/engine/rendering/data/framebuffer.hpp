#pragma once
#include <application/application.hpp>
#include <rendering/data/texture.hpp>
#include <rendering/data/renderbuffer.hpp>
#include <variant>
#include <unordered_map>

/**
 * @file framebuffer.hpp
 */

namespace legion::rendering
{
    using attachment = std::variant<std::monostate, texture_handle, renderbuffer>;
    static const attachment invalid_attachment = std::monostate();
    /**@class framebuffer
     * @brief Low level framebuffer.
     */
    struct framebuffer
    {
    private:
        // Managed resource with the framebuffer id, also has the responsibility of deleting the framebuffer after all copies of this framebuffer have been destroyed.
        common::managed_resource<app::gl_id> m_id = common::managed_resource<app::gl_id>(nullptr);
        // Type of framebuffer, either read, write or both.
        GLenum m_target = GL_FRAMEBUFFER;

        // Attachments to the framebuffer. Can be either texture handles or renderbuffers.
        std::unordered_map<GLenum, attachment> m_attachments;

    public:

        /**@brief Faux constructor. To prevent unnecessary GPU allocations the default constructor doesn't actually create a framebuffer.
         *        This means that default initialized framebuffers are invalid temporary objects until they get properly initialized.
         */
        framebuffer() = default;

        /**@brief Main allocating constructor. This constructor actually creates the GPU side framebuffer.
         * @note Read more at <a href="http://docs.gl/gl4/glBindFramebuffer">docs.gl.</a>
         * @param target Framebuffer target for general operations.
         */
        explicit framebuffer(GLenum target);

        /**@brief Check if the framebuffer is valid and complete.
         * @return std::pair<bool, std::string> First will be a boolean that will be true if the framebuffer is valid, the second will be the message about the state of the framebuffer.
         */
        std::pair<bool, std::string> verify() const;

        /**@brief Returns the rendering API type/target of the framebuffer. Useful for low level native rendering.
         */
        GLenum target() const;

        /**@brief Returns the rendering API id of the framebuffer. Useful for low level native rendering.
         */
        app::gl_id id() const;

        /**@brief Bind the framebuffer to the current context. Useful for low level native rendering.
         */
        void bind() const;

        /**@brief Attach a renderbuffer to a certain attachment-point of this framebuffer.
         * @param rbo Renderbuffer to attach.
         * @param attachment Attachment-point to attach the renderbuffer to.
         */
        void attach(renderbuffer rbo, GLenum attachment);

        /**@brief Attach a unknown type attachment to a certain attachment-point of this framebuffer.
         * @param any Attachment to attach.
         * @param attachment Attachment-point to attach the renderbuffer to.
         */
        void attach(attachment att, GLenum attachment);

        /**@brief Attach a texture to a certain attachment-point of this framebuffer.
         * @param texture Texture handle of the texture to attach.
         * @param attachment Attachment-point to attach the texture to.
         */
        void attach(texture_handle texture, GLenum attachment);

        void detach(GLenum attachment);

        /**@brief Get the attachment of a certain attachment-point.
         * @param attachment Attachment-point to fetch the attachment for.
         * @return const std::any& An std::any that should be any_castable to either a texture handle or a renderbuffer if an attachment was active, the std::any will be empty otherwise.
         */
        L_NODISCARD const attachment& getAttachment(GLenum attachment) const;

        /**@brief Release the framebuffer from the current context. Useful for low level native rendering.
         */
        void release() const;

        /**@brief Check if the framebuffer was generated or not.
         */
        L_NODISCARD operator bool() const;
    };
}
