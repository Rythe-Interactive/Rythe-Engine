#pragma once
#include <rendering/data/postprocessingeffect.hpp>

namespace legion::rendering
{
    class PostProcessingBlur : public PostProcessingEffect<PostProcessingBlur>
    {
    private:
        shader_handle m_blurShader;

    public:

        void setup(app::window& context) override
        {
            using namespace legion::core::fs::literals;
            m_blurShader = rendering::ShaderCache::create_shader("blur_shader", "assets://shaders/blurcolor.shs"_view);
            addRenderPass<&PostProcessingBlur::renderPass>();
        }

        void renderPass(texture_handle colortexture, texture_handle depthtexture)
        {
            m_blurShader.bind();
            m_blurShader.get_uniform<texture_handle>("screenTexture").set_value(colortexture);
            renderQuad();
            m_blurShader.release();
        }

    };


}
