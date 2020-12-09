#include <rendering/pipeline/default/stages/framebufferresizestage.hpp>

namespace legion::rendering
{
    void FramebufferResizeStage::setRenderScale(float renderScale)
    {
        m_renderScale.store(renderScale, std::memory_order_release);
    }

    float FramebufferResizeStage::getRenderScale()
    {
        return m_renderScale.load(std::memory_order_acquire);
    }

    void FramebufferResizeStage::setup(app::window& context)
    {
        float renderScale = m_renderScale.load(std::memory_order_acquire);
        m_framebufferSize = context.framebufferSize();
        m_framebufferSize.x = math::max((int)(m_framebufferSize.x * renderScale), 1);
        m_framebufferSize.y = math::max((int)(m_framebufferSize.y * renderScale), 1);

        m_colorTexture = TextureCache::create_texture("color_image", m_framebufferSize, {
        texture_type::two_dimensional, channel_format::eight_bit, texture_format::rgb,
        texture_components::rgb, true, true, texture_mipmap::linear, texture_mipmap::linear,
        texture_wrap::repeat, texture_wrap::repeat, texture_wrap::repeat });
        m_depthTexture = TextureCache::create_texture("depth_image", m_framebufferSize, {
        texture_type::two_dimensional, channel_format::eight_bit, texture_format::depth,
        texture_components::depth, true, true, texture_mipmap::linear, texture_mipmap::linear,
        texture_wrap::repeat, texture_wrap::repeat, texture_wrap::repeat });
        m_stencilbuffer = renderbuffer(GL_STENCIL, m_framebufferSize);

        auto fbo = getFramebuffer("main");
        fbo.attach(m_colorTexture, GL_COLOR_ATTACHMENT0);
        fbo.attach(m_depthTexture, GL_DEPTH_ATTACHMENT);
        fbo.attach(m_stencilbuffer, GL_STENCIL_ATTACHMENT);
    }

    void FramebufferResizeStage::render(app::window& context, camera& cam, const camera::camera_input& camInput, time::span deltaTime)
    {

    }

}
