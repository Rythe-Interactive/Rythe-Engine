#pragma once
#include <rendering/data/postprocessingeffect.hpp>


namespace legion::rendering
{
    class PostProcessingBloom : public PostProcessingEffect<PostProcessingBloom>
    {
    private:
        shader_handle m_brightnessThresholdShader;
        shader_handle m_gaussianBlurShader;
        shader_handle m_combineShader;
        //m_shader3
        texture_handle m_brightTexture;
        framebuffer m_pingpongFrameBuffers[2];
        texture_handle m_pingpongTextureBuffers[2];

        texture_import_settings settings{
              texture_type::two_dimensional,
              channel_format::eight_bit,
              texture_format::rgb,
              texture_components::rgb,
              true,
              true,
              texture_mipmap::linear,
              texture_mipmap::linear,
              texture_wrap::repeat,
              texture_wrap::repeat,
              texture_wrap::repeat
        };

    public:

        void setup(app::window& context) override
        {
            using namespace legion::core::fs::literals;
            //get shaders
            m_brightnessThresholdShader = rendering::ShaderCache::create_shader("brightnessthreshold_shader", "assets://shaders/brightnessthresholdshader.shs"_view);
            m_gaussianBlurShader = rendering::ShaderCache::create_shader("gaussianblur_shader", "assets://shaders/gaussianblurshader.shs"_view);
            m_combineShader = rendering::ShaderCache::create_shader("m_combineShader", "assets://shaders/bloomshader.shs"_view);

            for (int i = 0; i < 2; i++)
            {
                m_pingpongFrameBuffers[i] = framebuffer(GL_FRAMEBUFFER);
                m_pingpongTextureBuffers[i] = rendering::TextureCache::create_texture("blurTexture" + i, context.size(), settings);
                m_pingpongFrameBuffers[i].attach(m_pingpongTextureBuffers[i], GL_COLOR_ATTACHMENT0);
            }

            addRenderPass<&PostProcessingBloom::renderPass>();
        }

        void renderPass(framebuffer& fbo, texture_handle colortexture, texture_handle depthtexture)
        {
            if (!m_brightTexture)
            {
                m_brightTexture = TextureCache::create_texture("brightTexture", colortexture.get_texture().size(), settings);
            }

            math::ivec2 framebufferSize = colortexture.get_texture().size();
            if (m_brightTexture.get_texture().size() != framebufferSize)
                m_brightTexture.get_texture().resize(framebufferSize);

            fbo.attach(m_brightTexture, GL_COLOR_ATTACHMENT1);

            //Brightness threshold stage
            fbo.bind();
            uint attachments[2] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1 };
            glDrawBuffers(2, attachments);

            m_brightnessThresholdShader.bind();
            m_brightnessThresholdShader.get_uniform<texture_handle>("screenTexture").set_value(colortexture);
            renderQuad();
            m_brightnessThresholdShader.release();

            uint attachment = GL_COLOR_ATTACHMENT0;
            glDrawBuffers(1, &attachment);

            fbo.release();
            fbo.detach(GL_COLOR_ATTACHMENT1);

            //Gaussian blur stage
            bool horizontal = true, first_iteration = true;
            int amount = 10;
            m_gaussianBlurShader.bind();

            if (m_pingpongTextureBuffers[0].get_texture().size() != framebufferSize)
            {
                m_pingpongTextureBuffers[0].get_texture().resize(framebufferSize);
                m_pingpongTextureBuffers[1].get_texture().resize(framebufferSize);
            }

            for (uint i = 0; i < amount; i++)
            {
                m_gaussianBlurShader.get_uniform<bool>("horizontal").set_value(horizontal);

                if (first_iteration)
                {
                    first_iteration = false;
                    m_gaussianBlurShader.get_uniform<texture_handle>("image").set_value(m_brightTexture);
                }
                else
                {
                    m_gaussianBlurShader.get_uniform<texture_handle>("image").set_value(m_pingpongTextureBuffers[!horizontal]);
                }

                m_pingpongFrameBuffers[horizontal].bind();
                renderQuad();
                horizontal = !horizontal;
            }
            m_pingpongFrameBuffers[horizontal].release();
            m_gaussianBlurShader.release();

            //Add m_brightTexture to colorTexture
            //Render to GL_COLOR_ATTACHMENT0

            fbo.bind();
            m_combineShader.bind();
            m_combineShader.get_uniform<texture_handle>("screenTexture").set_value(colortexture);
            m_combineShader.get_uniform<texture_handle>("brightTexture").set_value(m_pingpongTextureBuffers[!horizontal]);
            renderQuad();
            m_combineShader.release();
            fbo.release();
        }
    };
}
