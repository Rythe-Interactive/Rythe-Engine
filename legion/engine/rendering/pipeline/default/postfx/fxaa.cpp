#include <rendering/pipeline/default/postfx/fxaa.hpp>

namespace legion::rendering
{
    void FXAA::setup(app::window& context)
    {
        using namespace legion::core::fs::literals;
        m_fxaaShader = ShaderCache::create_shader("fxaa", "engine://shaders/fxaa.shs"_view);
        addRenderPass<&FXAA::renderPass>();
    }

    void FXAA::renderPass(framebuffer& fbo, texture_handle colortexture, texture_handle depthtexture, time::span deltaTime)
    {
        static id_type texelSizeId = nameHash("texelSize");
        static id_type blurConstraintId = nameHash("blurConstraint");
        static id_type reductionScaleId = nameHash("reductionScale");
        static id_type reductionConstraintId = nameHash("reductionConstraint");

        fbo.bind();
        m_fxaaShader.bind();


        auto renderSize = colortexture.get_texture().size();
        m_fxaaShader.get_uniform<math::vec2>(texelSizeId).set_value(math::vec2(1.f / renderSize.x, 1.f / renderSize.y));

        m_fxaaShader.get_uniform<float>(blurConstraintId).set_value(8.f);
        m_fxaaShader.get_uniform<float>(reductionScaleId).set_value(1.f/8.f);
        m_fxaaShader.get_uniform<float>(reductionConstraintId).set_value(1.f/128.f);
        m_fxaaShader.get_uniform_with_location<texture_handle>(SV_SCENECOLOR).set_value(colortexture);

        renderQuad();
        m_fxaaShader.release();
        fbo.release();
    }
}
