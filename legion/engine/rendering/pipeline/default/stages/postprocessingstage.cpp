#include <rendering/pipeline/default/stages/postprocessingstage.hpp>

namespace legion::rendering
{

    std::multimap<priority_type, std::unique_ptr<PostProcessingEffectBase>, std::greater<>> PostProcessingStage::m_effects;


    void PostProcessingStage::setup(app::window& context)
    {
        OPTICK_EVENT();
        using namespace legion::core::fs::literals;

        app::context_guard guard(context);

        m_screenQuad = screen_quad::generate();

        m_drawFBO = framebuffer(GL_FRAMEBUFFER);

        m_swapTexture = TextureCache::create_texture("color_swap_image", math::ivec2(1, 1), {
        texture_type::two_dimensional, channel_format::float_hdr, texture_format::rgba_hdr,
        texture_components::rgb, true, true, texture_mipmap::linear, texture_mipmap::linear,
        texture_wrap::repeat, texture_wrap::repeat, texture_wrap::repeat });

        m_screenShader = ShaderCache::create_shader("screen shader", "engine://shaders/screenshader.shs"_view);
    }

    void PostProcessingStage::render(app::window& context, camera& cam, const camera::camera_input& camInput, time::span deltaTime)
    {
        OPTICK_EVENT();
        static id_type mainId = nameHash("main");

        auto fbo = getFramebuffer(mainId);
        if (!fbo)
        {
            log::error("Main frame buffer is missing.");
            abort();
            return;
        }

        app::context_guard guard(context);
        if (!guard.contextIsValid())
        {
            abort();
            return;
        }

        auto [valid, message] = fbo->verify();
        if (!valid)
        {
            log::error("Main frame buffer isn't complete: {}", message);
            abort();
            return;
        }

        auto colorAttachment = fbo->getAttachment(FRAGMENT_ATTACHMENT);
        if (std::holds_alternative<std::monostate>(colorAttachment))
        {
            log::error("Color attachment was not found.");
            return;
        }
        if (!std::holds_alternative<texture_handle>(colorAttachment))
        {
            log::error("Color attachment needs to be a texture to be able to use it for post processing.");
            return;
        }

        texture_handle texture = std::get<texture_handle>(colorAttachment);

        math::ivec2 attachmentSize = texture.get_texture().size();

        bool stencil = false;
        auto depthAttachment = fbo->getAttachment(GL_DEPTH);
        if (std::holds_alternative<std::monostate>(depthAttachment))
        {
            stencil = true;
            depthAttachment = fbo->getAttachment(GL_DEPTH_STENCIL);
        }

        texture_handle depthTexture = invalid_texture_handle;
        if (std::holds_alternative<texture_handle>(depthAttachment))
            depthTexture = std::get<texture_handle>(depthAttachment);
        glDisable(GL_DEPTH_TEST);

        fbo->bind();
        uint attachment = FRAGMENT_ATTACHMENT;
        glDrawBuffers(1, &attachment);
        fbo->release();

        for (auto& [_, effect] : m_effects)
        {
            OPTICK_EVENT("Rendering effect");
            OPTICK_TAG("Effect", effect->getName().c_str());

            if (!effect->isInitialized()) effect->init(context);
            for (auto& pass : effect->renderPasses)
            {
                OPTICK_EVENT("Effect pass");
                pass.invoke(*fbo, m_pipeline, cam, camInput, deltaTime);
            }
        }

        rendering::shader::release();

        glEnable(GL_DEPTH_TEST);
    }

    priority_type PostProcessingStage::priority()
    {
        return post_fx_priority;
    }

}
