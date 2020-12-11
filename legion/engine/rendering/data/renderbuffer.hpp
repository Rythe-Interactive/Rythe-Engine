#pragma once
#include <application/application.hpp>

/**@file renderbuffer.hpp
 */
namespace legion::rendering
{
    /**@class renderbuffer
     * @brief Low level renderbuffer.
     */
    struct renderbuffer
    {
    private:
        // Managed resource with the renderbuffer id, also has the responsibility of deleting the renderbuffer after all copies of this renderbuffer have been destroyed.
        common::managed_resource<app::gl_id> m_id = common::managed_resource<app::gl_id>(nullptr);
        // Amount of samples of the renderbuffer. 0 if the renderbuffer isn't multi-sampled.
        uint m_samples = 0;
        GLenum m_internalformat;
    public:

        /**@brief Faux constructor. To prevent unnecessary GPU allocations the default constructor doesn't actually create a renderbuffer.
         *        This means that default initialized renderbuffers are invalid temporary objects until they get properly initialized.
         */
        renderbuffer() = default;

        /**@brief Main allocating constructor. This constructor actually creates the GPU side renderbuffer.
         * @note Read more at <a href="http://docs.gl/gl4/glRenderbufferStorage">docs.gl.</a>
         * @param internalformat Internal format of the renderbuffer. eg: GL_DEPTH_COMPONENT, GL_DEPTH24_STENCIL8, GL_STENCIL_INDEX
         * @param resolution Resolution of the renderbuffer.
         * @param samples Amount of samples to use for multi-sampling. Leave this 0 to disable multi-sampling.
         */
        renderbuffer(GLenum internalformat, math::ivec2 resolution, uint samples = 0);

        /**@brief Main allocating constructor. This constructor actually creates the GPU side renderbuffer.
         * @note Read more at <a href="http://docs.gl/gl4/glRenderbufferStorage">docs.gl.</a>
         * @param internalformat Internal format of the renderbuffer. eg: GL_DEPTH_COMPONENT, GL_DEPTH24_STENCIL8, GL_STENCIL_INDEX
         * @param width Width of the renderbuffer.
         * @param height Height of the renderbuffer.
         * @param samples Amount of samples to use for multi-sampling. Leave this 0 to disable multi-sampling.
         */
        renderbuffer(GLenum internalformat, int width, int height, uint samples = 0);


        /**@brief Main non allocating constructor. This constructor actually creates the GPU side renderbuffer.
         * @note Read more at <a href="http://docs.gl/gl4/glRenderbufferStorage">docs.gl.</a>
         * @param internalformat Internal format of the renderbuffer. eg: GL_DEPTH_COMPONENT, GL_DEPTH24_STENCIL8, GL_STENCIL_INDEX
         * @param samples Amount of samples to use for multi-sampling. Leave this 0 to disable multi-sampling.
         * @note This constructor doesn't actually allocate any VRAM to this renderbuffer yet and requires a resize call in order to allocate memory for it.
         */
        renderbuffer(GLenum internalformat, uint samples = 0);

        /**@brief Bind the renderbuffer to the current context. Useful for low level native rendering.
         */
        void bind() const;

        /**@brief Release the renderbuffer from the current context. Useful for low level native rendering.
         */
        static void release();

        /**@brief Returns the rendering API id of the renderbuffer. Useful for low level native rendering.
         */
        L_NODISCARD app::gl_id id() const;

        /**@brief Returns the multi-sampling sample count of the renderbuffer. Useful for low level native rendering.
         */
        L_NODISCARD size_type samples() const;

        /**@brief Returns the internal format of the renderbuffer. Useful for low level native rendering.
         */
        L_NODISCARD GLenum format() const;

        /**@brief Returns the current resolution of the renderbuffer.
         */
        L_NODISCARD math::ivec2 size() const;

        /**@brief Resizes the renderbuffer to a new resolution. This reallocates VRAM and thus invalidates all data in the renderbuffer.
         */
        void resize(math::ivec2 newSize) const;

    };
}
