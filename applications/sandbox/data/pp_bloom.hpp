#pragma once
#include <rendering/data/postprocessingeffect.hpp>


namespace legion::rendering
{
    class PostProcessingBloom : public PostProcessingEffect<PostProcessingBloom>
    {
    private:
        shader_handle m_BrightnessThresholdShader;
        //m_shader2
        //m_shader3
        texture_handle m_brightTexture;
    public:

        void setup(app::window& context) override
        {
            using namespace legion::core::fs::literals;
            //get shaders
            m_BrightnessThresholdShader = rendering::ShaderCache::create_shader("brightnessthreshold_shader", "assets://shaders/brightnessthresholdshader.shs"_view);
            addRenderPass<&PostProcessingBloom::renderPass>();
        }

        void renderPass(framebuffer& fbo, texture_handle colortexture, texture_handle depthtexture)
        {
            if(!m_brightTexture)
            {
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

                m_brightTexture = TextureCache::create_texture("brightTexture", colortexture.get_texture().size(), settings);
            }
            fbo.attach(m_brightTexture, GL_COLOR_ATTACHMENT1);

            fbo.bind();
            unsigned int attachments[2] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1 };
            glDrawBuffers(2, attachments);

            m_BrightnessThresholdShader.bind();
            m_BrightnessThresholdShader.get_uniform<texture_handle>("screenTexture").set_value(colortexture);
            renderQuad();
            m_BrightnessThresholdShader.release();
            fbo.release();

            fbo.detach(GL_COLOR_ATTACHMENT1);

            fbo.bind();
            auto scrnshader = rendering::ShaderCache::get_handle("screen shader");
            scrnshader.bind();
            scrnshader.get_uniform<texture_handle>("screenTexture").set_value(m_brightTexture);
            renderQuad();
            scrnshader.release();
            fbo.release();
        }
    };
}
