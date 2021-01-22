#include <rendering/pipeline/default/postfx/bloom.hpp>
#include <rendering/systems/renderer.hpp>

namespace legion::rendering
{
    void Bloom::setup(app::window& context)
    {
        using namespace legion::core::fs::literals;
        // Create all the shaders needed.
        m_brightnessThresholdShader = rendering::ShaderCache::create_shader("bloom brightness threshold", "engine://shaders/bloombrightnessthreshold.shs"_view);
        m_gaussianBlurShader = rendering::ShaderCache::create_shader("gaussian blur", "engine://shaders/gaussianblur.shs"_view);
        m_combineShader = rendering::ShaderCache::create_shader("bloom combine", "engine://shaders/bloomcombine.shs"_view);
        m_historyMixShader = rendering::ShaderCache::create_shader("bloom history mix", "engine://shaders/bloomhistorymix.shs"_view);

        // Creating 2 framebuffers to pingpong texturs with. (used for blurring)
        for (int i = 0; i < 2; i++)
        {
            m_pingpongFrameBuffers[i] = framebuffer(GL_FRAMEBUFFER);
            m_pingpongTextureBuffers[i] = rendering::TextureCache::create_texture("blurTexture" + std::to_string(i), context.size(), settings);
            m_pingpongFrameBuffers[i].attach(m_pingpongTextureBuffers[i], FRAGMENT_ATTACHMENT);
        }
        // Adding itself to the post processing renderpass.
        addRenderPass<&Bloom::renderPass>();
    }

    void Bloom::seperateOverdraw(framebuffer& fbo, texture_handle colortexture, texture_handle overdrawtexture)
    {
        // Brightness threshold stage
        fbo.bind();
        // Create 2 color attachments and add them the buffer.
        uint attachments[4] = { FRAGMENT_ATTACHMENT, NORMAL_ATTACHMENT, POSITION_ATTACHMENT, OVERDRAW_ATTACHMENT };
        glDrawBuffers(4, attachments);

        // Bind and assign the brightness threshold shader.
        m_brightnessThresholdShader.bind();
        m_brightnessThresholdShader.get_uniform_with_location<texture_handle>(SV_SCENECOLOR).set_value(colortexture);
        m_brightnessThresholdShader.get_uniform_with_location<texture_handle>(SV_HDROVERDRAW).set_value(overdrawtexture);
        // Render onto the quad.
        renderQuad();
        // Release the shader.
        m_brightnessThresholdShader.release();

        // release the brightness texture from the renderbuffer.
        uint attachment = FRAGMENT_ATTACHMENT;
        glDrawBuffers(1, &attachment);

        // detach the second color attachment from the framebuffer.
        fbo.release();
    }

#define itterations 2
#define kernelsize 5

    texture_handle Bloom::blurOverdraw(const math::ivec2& framebufferSize, texture_handle overdrawtexture)
    {
        // Gaussian blur stage
        bool horizontal = true, first_iteration = true;

        // Bind and assign the viewport size to the gaussian blur shader.
        m_gaussianBlurShader.bind();
        m_gaussianBlurShader.get_uniform_with_location<math::ivec2>(SV_VIEWPORT).set_value(framebufferSize);
        m_gaussianBlurShader.get_uniform<int>("kernelsize").set_value(kernelsize);

        // Resize the pingpong textures.
        if (m_pingpongTextureBuffers[0].get_texture().size() != framebufferSize)
        {
            m_pingpongTextureBuffers[0].get_texture().resize(framebufferSize);
            m_pingpongTextureBuffers[1].get_texture().resize(framebufferSize);
        }

        // This loop blurs the brightness threshold texture an x amount of times.
        for (uint i = 0; i < itterations * 2; i++)
        {
            // Bind the first pongpong framebuffer.
            m_pingpongFrameBuffers[horizontal].bind();
            // Bind and assign the gaussian blur, the assigned value tells it if it is blurring horizontal or vertical.
            m_gaussianBlurShader.bind();
            m_gaussianBlurShader.get_uniform<bool>("horizontal").set_value(horizontal);
            // Setup the first shader run, assigning the brightness threshold texture to the blur shader.
            if (first_iteration)
            {
                first_iteration = false;
                m_gaussianBlurShader.get_uniform_with_location<texture_handle>(SV_SCENECOLOR).set_value(overdrawtexture);
            }
            else
            {
                m_gaussianBlurShader.get_uniform_with_location<texture_handle>(SV_SCENECOLOR).set_value(m_pingpongTextureBuffers[!horizontal]);
            }
            // Render the texture onto the quad, using the shader.
            renderQuad();
            horizontal = !horizontal;
        }
        // release the last used framebuffer and shader.
        m_pingpongFrameBuffers[horizontal].release();
        m_gaussianBlurShader.release();

        return m_pingpongTextureBuffers[!horizontal];
    }

    void Bloom::historyMixOverdraw(framebuffer& fbo, texture_handle overdrawtexture)
    {
        fbo.bind();
        uint attachments[4] = { FRAGMENT_ATTACHMENT, NORMAL_ATTACHMENT, POSITION_ATTACHMENT, OVERDRAW_ATTACHMENT };
        glDrawBuffers(4, attachments);

        static id_type historySamplerId = nameHash("overdrawHistory");
        static id_type historyMetaId = nameHash("HDR overdraw history");

        auto* pipeline = Renderer::getCurrentPipeline();
        texture_handle* historyTexture = pipeline->get_meta<texture_handle>(historyMetaId);
        if (!historyTexture)
            return;

        m_historyMixShader.bind();
        m_historyMixShader.get_uniform<texture_handle>(historySamplerId).set_value(*historyTexture);
        m_historyMixShader.get_uniform_with_location<texture_handle>(SV_HDROVERDRAW).set_value(overdrawtexture);
        renderQuad();
        m_historyMixShader.release();


        uint attachment = FRAGMENT_ATTACHMENT;
        glDrawBuffers(1, &attachment);
        fbo.release();
    }

    void Bloom::combineImages(framebuffer& fbo, texture_handle colortexture, texture_handle overdrawtexture)
    {
        // Combining phase.
        // bind the combining shader.
        fbo.bind();
        m_combineShader.bind();

        // Assign the lighting data texture and blurred brightness threshold texture.
        m_combineShader.get_uniform_with_location<texture_handle>(SV_SCENECOLOR).set_value(colortexture);
        m_combineShader.get_uniform_with_location<texture_handle>(SV_HDROVERDRAW).set_value(overdrawtexture);
        // Render it onto the quad.
        renderQuad();
        // Release both the combining shader and framebuffer.
        m_combineShader.release();
        fbo.release();
    }

    void Bloom::renderPass(framebuffer& fbo, texture_handle colortexture, texture_handle depthtexture, time::span deltaTime)
    {
        // If a brightness threshold texture had not been created yet, create one.
        texture_handle overdrawTexture;

        {
            auto attachment = fbo.getAttachment(OVERDRAW_ATTACHMENT);
            if (std::holds_alternative<texture_handle>(attachment))
                overdrawTexture = std::get<texture_handle>(attachment);
        }

        // Get brightest parts of the scene and append to overdraw buffer.
        seperateOverdraw(fbo, colortexture, overdrawTexture);

        // Gets the size of the lighting data texture.
        math::ivec2 framebufferSize = colortexture.get_texture().size();

        // Mix slight part of the previous frame overdraw into the current frame to reduce flickering and introduce slight trail when it's dark.
        historyMixOverdraw(fbo, overdrawTexture);

        // Blur the overdraw buffer.
        texture_handle blurredImage = blurOverdraw(framebufferSize, overdrawTexture);

        // Recombine the overdraw texture with the scene color.
        combineImages(fbo, colortexture, blurredImage);
    }

}
