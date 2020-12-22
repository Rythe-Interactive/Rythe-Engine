#include <rendering/pipeline/default/stages/framebufferresizestage.hpp>

namespace legion::rendering
{
    std::atomic<float> FramebufferResizeStage::m_renderScale = 1.f;


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
        OPTICK_EVENT();
        float renderScale = m_renderScale.load(std::memory_order_acquire);
        m_framebufferSize = context.framebufferSize();
        m_framebufferSize.x = math::max((int)(m_framebufferSize.x * renderScale), 1);
        m_framebufferSize.y = math::max((int)(m_framebufferSize.y * renderScale), 1);

        app::context_guard guard(context);

        m_colorTexture = TextureCache::create_texture("color_image", m_framebufferSize, {
        texture_type::two_dimensional, channel_format::float_hdr, texture_format::rgba_hdr,
        texture_components::rgb, true, true, texture_mipmap::linear, texture_mipmap::linear,
        texture_wrap::repeat, texture_wrap::repeat, texture_wrap::repeat });

        m_depthStencilTexture = TextureCache::create_texture("depth_stencil_image", m_framebufferSize, {
        texture_type::two_dimensional, channel_format::depth_stencil, texture_format::depth_stencil,
        texture_components::depth_stencil, true, true, texture_mipmap::linear, texture_mipmap::linear,
        texture_wrap::repeat, texture_wrap::repeat, texture_wrap::repeat });

        //m_stencilbuffer = renderbuffer(GL_STENCIL_INDEX8, m_framebufferSize);

        auto fbo = getFramebuffer("main");

        if (!fbo)
        {
            log::error("Main frame buffer is missing.");
            return;
        }

        fbo->attach(m_colorTexture, GL_COLOR_ATTACHMENT0);
        fbo->attach(m_depthStencilTexture, GL_DEPTH_STENCIL_ATTACHMENT);
    }

    void FramebufferResizeStage::render(app::window& context, camera& cam, const camera::camera_input& camInput, time::span deltaTime)
    {
        OPTICK_EVENT();
        float renderScale = m_renderScale.load(std::memory_order_acquire);
        math::ivec2 framebufferSize = context.framebufferSize();
        framebufferSize.x *= renderScale;
        framebufferSize.y *= renderScale;

        if (framebufferSize.x == 0 || framebufferSize.y == 0)
        {
            abort();
            return;
        }

        if (framebufferSize != m_framebufferSize)
        {
            app::context_guard guard(context);

            m_framebufferSize = framebufferSize;
            m_colorTexture.get_texture().resize(framebufferSize);
            m_depthStencilTexture.get_texture().resize(framebufferSize);

            auto fbo = getFramebuffer("main");

            if (!fbo)
            {
                log::error("Main frame buffer is missing.");
                abort();
                return;
            }

            fbo->attach(m_colorTexture, GL_COLOR_ATTACHMENT0);
            fbo->attach(m_depthStencilTexture, GL_DEPTH_STENCIL_ATTACHMENT);
        }
    }

    priority_type FramebufferResizeStage::priority()
    {
        return setup_priority;
    }

}
