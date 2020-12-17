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
        bool textureInitialized = false;
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
            if(!textureInitialized)
            {
                textureInitialized = true;
                texture_import_settings settings{
               colortexture.get_texture().type,
               colortexture.get_texture().fileFormat,
               colortexture.get_texture().format,
               colortexture.get_texture().channels,
               false,
               false,
               texture_mipmap::linear,
               texture_mipmap::linear,
               texture_wrap::edge_clamp,
               texture_wrap::edge_clamp,
               texture_wrap::edge_clamp
                };
                m_brightTexture = TextureCache::create_texture("brightTexture", colortexture.get_data().size, settings);
            }
            fbo.attach(m_brightTexture, GL_COLOR_ATTACHMENT1);

            fbo.bind();
            m_BrightnessThresholdShader.bind();
            m_BrightnessThresholdShader.get_uniform<texture_handle>("screenTexture").set_value(m_brightTexture);
            renderQuad();
            m_BrightnessThresholdShader.release();
            fbo.release();
        }
    };
}
