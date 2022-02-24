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

        m_colorTexture[0] = TextureCache::create_texture("color_image0", m_framebufferSize, {
        texture_type::two_dimensional, channel_format::float_hdr, texture_format::rgba_hdr,
        texture_components::rgb, true, true, texture_mipmap::linear, texture_mipmap::linear,
        texture_wrap::repeat, texture_wrap::repeat, texture_wrap::repeat });

        m_colorTexture[1] = TextureCache::create_texture("color_image1", m_framebufferSize, {
        texture_type::two_dimensional, channel_format::float_hdr, texture_format::rgba_hdr,
        texture_components::rgb, true, true, texture_mipmap::linear, texture_mipmap::linear,
        texture_wrap::repeat, texture_wrap::repeat, texture_wrap::repeat });

        m_normalTexture[0] = TextureCache::create_texture("normal_image0", m_framebufferSize, {
        texture_type::two_dimensional, channel_format::float_hdr, texture_format::rgb_hdr,
        texture_components::rgb, true, true, texture_mipmap::linear, texture_mipmap::linear,
        texture_wrap::repeat, texture_wrap::repeat, texture_wrap::repeat });

        m_normalTexture[1] = TextureCache::create_texture("normal_image1", m_framebufferSize, {
        texture_type::two_dimensional, channel_format::float_hdr, texture_format::rgb_hdr,
        texture_components::rgb, true, true, texture_mipmap::linear, texture_mipmap::linear,
        texture_wrap::repeat, texture_wrap::repeat, texture_wrap::repeat });

        m_positionTexture[0] = TextureCache::create_texture("position_image0", m_framebufferSize, {
        texture_type::two_dimensional, channel_format::float_hdr, texture_format::rgb_hdr,
        texture_components::rgb, true, true, texture_mipmap::linear, texture_mipmap::linear,
        texture_wrap::repeat, texture_wrap::repeat, texture_wrap::repeat });

        m_positionTexture[1] = TextureCache::create_texture("position_image1", m_framebufferSize, {
        texture_type::two_dimensional, channel_format::float_hdr, texture_format::rgb_hdr,
        texture_components::rgb, true, true, texture_mipmap::linear, texture_mipmap::linear,
        texture_wrap::repeat, texture_wrap::repeat, texture_wrap::repeat });

        m_overdrawTexture[0] = TextureCache::create_texture("overdraw_image0", m_framebufferSize, {
        texture_type::two_dimensional, channel_format::float_hdr, texture_format::rgba_hdr,
        texture_components::rgb, true, true, texture_mipmap::linear, texture_mipmap::linear,
        texture_wrap::mirror, texture_wrap::mirror, texture_wrap::mirror });

        m_overdrawTexture[1] = TextureCache::create_texture("overdraw_image1", m_framebufferSize, {
        texture_type::two_dimensional, channel_format::float_hdr, texture_format::rgba_hdr,
        texture_components::rgb, true, true, texture_mipmap::linear, texture_mipmap::linear,
        texture_wrap::mirror, texture_wrap::mirror, texture_wrap::mirror });

        m_depthStencilTexture[0] = TextureCache::create_texture("depth_stencil_image0", m_framebufferSize, {
        texture_type::two_dimensional, channel_format::depth_stencil, texture_format::depth_stencil,
        texture_components::depth_stencil, true, true, texture_mipmap::linear, texture_mipmap::linear,
        texture_wrap::repeat, texture_wrap::repeat, texture_wrap::repeat });

        m_depthStencilTexture[1] = TextureCache::create_texture("depth_stencil_image1", m_framebufferSize, {
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

        fbo->attach(m_colorTexture[0], FRAGMENT_ATTACHMENT);
        fbo->attach(m_normalTexture[0], NORMAL_ATTACHMENT);
        fbo->attach(m_positionTexture[0], POSITION_ATTACHMENT);
        fbo->attach(m_overdrawTexture[0], OVERDRAW_ATTACHMENT);
        fbo->attach(m_depthStencilTexture[0], GL_DEPTH_STENCIL_ATTACHMENT);

        fbo->bind();
        uint attachments[4] = { FRAGMENT_ATTACHMENT, NORMAL_ATTACHMENT, POSITION_ATTACHMENT, OVERDRAW_ATTACHMENT };
        glDrawBuffers(4, attachments);
        fbo->release();

        create_meta<texture_handle>("scene color history", m_colorTexture[1]);
        create_meta<texture_handle>("scene normal history", m_normalTexture[1]);
        create_meta<texture_handle>("scene position history", m_positionTexture[1]);
        create_meta<texture_handle>("HDR overdraw history", m_overdrawTexture[1]);
        create_meta<texture_handle>("scene depth history", m_depthStencilTexture[1]);
    }

    void FramebufferResizeStage::shutdown()
    {
       TextureCache::destroy_texture("color_image0");
        TextureCache::destroy_texture("color_image1");
        TextureCache::destroy_texture("normal_image0");
        TextureCache::destroy_texture("normal_image1");
        TextureCache::destroy_texture("position_image0");
        TextureCache::destroy_texture("position_image1");
        TextureCache::destroy_texture("overdraw_image0");
        TextureCache::destroy_texture("overdraw_image1");
        TextureCache::destroy_texture("depth_stencil_image0");
        TextureCache::destroy_texture("depth_stencil_image1");
    }

    void FramebufferResizeStage::render(app::window& context, camera& cam, const camera::camera_input& camInput, time::span deltaTime)
    {
        OPTICK_EVENT();
        static id_type sceneColorId = nameHash("scene color history");
        static id_type sceneNormalId = nameHash("scene normal history");
        static id_type scenePositionId = nameHash("scene position history");
        static id_type hdrOverdrawId = nameHash("HDR overdraw history");
        static id_type sceneDepthId = nameHash("scene depth history");
        static bool useTexture1 = false;

        float renderScale = m_renderScale.load(std::memory_order_acquire);
        math::int2 framebufferSize = context.framebufferSize();
        framebufferSize.x *= renderScale;
        framebufferSize.y *= renderScale;

        if (framebufferSize.x == 0 || framebufferSize.y == 0)
        {
            abort();
            return;
        }

        auto fbo = getFramebuffer("main");

        if (!fbo)
        {
            log::error("Main frame buffer is missing.");
            abort();
            return;
        }

        auto* sceneColormeta = get_meta<texture_handle>(sceneColorId);
        if (sceneColormeta)
            *sceneColormeta = m_colorTexture[!useTexture1];

        auto* sceneNormalmeta = get_meta<texture_handle>(sceneNormalId);
        if (sceneNormalmeta)
            *sceneNormalmeta = m_normalTexture[!useTexture1];

        auto* scenePositionmeta = get_meta<texture_handle>(scenePositionId);
        if (scenePositionmeta)
            *scenePositionmeta = m_positionTexture[!useTexture1];

        auto* hdrOverdrawmeta = get_meta<texture_handle>(hdrOverdrawId);
        if (hdrOverdrawmeta)
            *hdrOverdrawmeta = m_overdrawTexture[!useTexture1];

        auto* sceneDepthmeta = get_meta<texture_handle>(sceneDepthId);
        if (sceneDepthmeta)
            *sceneDepthmeta = m_depthStencilTexture[!useTexture1];

        {
            app::context_guard guard(context);

            if (framebufferSize != m_framebufferSize)
            {
                m_framebufferSize = framebufferSize;
                m_colorTexture[0].get_texture().resize(framebufferSize);
                m_colorTexture[1].get_texture().resize(framebufferSize);
                m_normalTexture[0].get_texture().resize(framebufferSize);
                m_normalTexture[1].get_texture().resize(framebufferSize);
                m_positionTexture[0].get_texture().resize(framebufferSize);
                m_positionTexture[1].get_texture().resize(framebufferSize);
                m_overdrawTexture[0].get_texture().resize(framebufferSize);
                m_overdrawTexture[1].get_texture().resize(framebufferSize);
                m_depthStencilTexture[0].get_texture().resize(framebufferSize);
                m_depthStencilTexture[1].get_texture().resize(framebufferSize);
            }

            fbo->attach(m_colorTexture[useTexture1], FRAGMENT_ATTACHMENT);
            fbo->attach(m_normalTexture[useTexture1], NORMAL_ATTACHMENT);
            fbo->attach(m_positionTexture[useTexture1], POSITION_ATTACHMENT);
            fbo->attach(m_overdrawTexture[useTexture1], OVERDRAW_ATTACHMENT);
            fbo->attach(m_depthStencilTexture[useTexture1], GL_DEPTH_STENCIL_ATTACHMENT);
        }

        useTexture1 = !useTexture1;
    }

    priority_type FramebufferResizeStage::priority()
    {
        return setup_priority + 1;
    }

}
