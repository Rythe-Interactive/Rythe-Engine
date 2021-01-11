#pragma once
#include <rendering/data/postprocessingeffect.hpp>


namespace legion::rendering
{
    /**
     * @class PostProcessingBloom
     * @brief A bloom post processing effect.
     */
    class PostProcessingBloom : public PostProcessingEffect<PostProcessingBloom>
    {
    private:
        shader_handle m_brightnessThresholdShader;
        shader_handle m_gaussianBlurShader;
        shader_handle m_combineShader;

        framebuffer m_pingpongFrameBuffers[2];
        texture_handle m_pingpongTextureBuffers[2];

        texture_import_settings settings{
              texture_type::two_dimensional,
              channel_format::float_hdr,
              texture_format::rgba_hdr,
              texture_components::rgb,
              true,
              true,
              texture_mipmap::linear,
              texture_mipmap::linear,
              texture_wrap::mirror,
              texture_wrap::mirror,
              texture_wrap::mirror
        };

    public:
        /**
         * @brief setup The setup function of the post processing effect.
         * @param context The current context that is being used inside of the effect.
         */
        void setup(app::window& context) override
        {
            using namespace legion::core::fs::literals;
            //Create all the shaders needed.
            m_brightnessThresholdShader = rendering::ShaderCache::create_shader("brightnessthreshold_shader", "assets://shaders/brightnessthresholdshader.shs"_view);
            m_gaussianBlurShader = rendering::ShaderCache::create_shader("gaussianblur_shader", "assets://shaders/gaussianblurshader.shs"_view);
            m_combineShader = rendering::ShaderCache::create_shader("m_combineShader", "assets://shaders/bloomshader.shs"_view);

            //Creating 2 framebuffers to pingpong texturs with. (used for blurring)
            for (int i = 0; i < 2; i++)
            {
                m_pingpongFrameBuffers[i] = framebuffer(GL_FRAMEBUFFER);
                m_pingpongTextureBuffers[i] = rendering::TextureCache::create_texture("blurTexture" + i, context.size(), settings);
                m_pingpongFrameBuffers[i].attach(m_pingpongTextureBuffers[i], FRAGMENT_ATTACHMENT);
            }
            //Adding itself to the post processing renderpass.
            addRenderPass<&PostProcessingBloom::renderPass>();
        }
        /**
         * @brief renderPass The function that is called every frame.
         * @param fbo The framebuffer used for this particular effect.
         * @param colortexture The lighting data gathered from previous renderpasses.
         * @param depthtexture The depth data gathered from previous renderpasses.
         * @param deltaTime Current delta time.
         */
        void renderPass(framebuffer& fbo, texture_handle colortexture, texture_handle depthtexture, time::span deltaTime)
        {
            //If a brightness threshold texture had not been created yet, create one.
            texture_handle overdrawTexture;

            {
                auto attachment = fbo.getAttachment(OVERDRAW_ATTACHMENT);
                if (std::holds_alternative<texture_handle>(attachment))
                    overdrawTexture = std::get<texture_handle>(attachment);
            }

            //Gets the size of the lighting data texture.
            math::ivec2 framebufferSize = colortexture.get_texture().size();
            //If brightness threshold texture has a different size than the lighting data, it will set it to the size of lighting data.
            if (overdrawTexture.get_texture().size() != framebufferSize)
                overdrawTexture.get_texture().resize(framebufferSize);

            //Brightness threshold stage
            fbo.bind();
            //Create 2 color attachments and add them the buffer.
            uint attachments[2] = { FRAGMENT_ATTACHMENT, OVERDRAW_ATTACHMENT };
            glDrawBuffers(2, attachments);

            //Bind and assign the brightness threshold shader.
            m_brightnessThresholdShader.bind();
            m_brightnessThresholdShader.get_uniform_with_location<texture_handle>(SV_SCENECOLOR).set_value(colortexture);
            m_brightnessThresholdShader.get_uniform_with_location<texture_handle>(SV_HDROVERDRAW).set_value(overdrawTexture);
            //Render onto the quad.
            renderQuad();
            //Release the shader.
            m_brightnessThresholdShader.release();

            //release the brightness texture from the renderbuffer.
            uint attachment = FRAGMENT_ATTACHMENT;
            glDrawBuffers(1, &attachment);

            //detach the second color attachment from the framebuffer.
            fbo.release();

            //Gaussian blur stage
            bool horizontal = true, first_iteration = true;
            int amount = 2;

            //Bind and assign the viewport size to the gaussian blur shader.
            m_gaussianBlurShader.bind();
            m_gaussianBlurShader.get_uniform_with_location<math::ivec2>(SV_VIEWPORT).set_value(framebufferSize);

            //Resize the pingpong textures.
            if (m_pingpongTextureBuffers[0].get_texture().size() != framebufferSize)
            {
                m_pingpongTextureBuffers[0].get_texture().resize(framebufferSize);
                m_pingpongTextureBuffers[1].get_texture().resize(framebufferSize);
            }

            //This loop blurs the brightness threshold texture an x amount of times.
            for (uint i = 0; i < amount; i++)
            {
                //Bind the first pongpong framebuffer.
                m_pingpongFrameBuffers[horizontal].bind();
                //Bind and assign the gaussian blur, the assigned value tells it if it is blurring horizontal or vertical.
                m_gaussianBlurShader.bind();
                m_gaussianBlurShader.get_uniform<bool>("horizontal").set_value(horizontal);
                //Setup the first shader run, assigning the brightness threshold texture to the blur shader.
                if (first_iteration)
                {
                    first_iteration = false;
                    m_gaussianBlurShader.get_uniform_with_location<texture_handle>(SV_SCENECOLOR).set_value(overdrawTexture);
                }
                else
                {
                    m_gaussianBlurShader.get_uniform_with_location<texture_handle>(SV_SCENECOLOR).set_value(m_pingpongTextureBuffers[!horizontal]);
                }
                //Render the texture onto the quad, using the shader.
                renderQuad();
                horizontal = !horizontal;
            }
            //release the last used framebuffer and shader.
            m_pingpongFrameBuffers[horizontal].release();
            m_gaussianBlurShader.release();

            //Combining phase.
            //bind the original framebuffer and the combining shader.
            fbo.bind();
            m_combineShader.bind();
            //Assign the lighting data texture and blurred brightness threshold texture.
            m_combineShader.get_uniform_with_location<texture_handle>(SV_SCENECOLOR).set_value(colortexture);
            m_combineShader.get_uniform_with_location<texture_handle>(SV_HDROVERDRAW).set_value(m_pingpongTextureBuffers[!horizontal]);
            //Render it onto the quad.
            renderQuad();
            //Release both the combining shader and framebuffer.
            m_combineShader.release();
            fbo.release();
        }
    };
}
