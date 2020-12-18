#pragma once
#include <rendering/data/postprocessingeffect.hpp>
#include <rendering/util/bindings.hpp>

namespace legion::rendering
{
    class Tonemapping : public PostProcessingEffect<Tonemapping>
    {
    private:
        shader_handle m_aces;

    public:

        void setup(app::window& context) override
        {
            using namespace legion::core::fs::literals;
            m_aces = rendering::ShaderCache::create_shader("aces tonemapping", "engine://shaders/aces.shs"_view);
            addRenderPass<&Tonemapping::renderPass>();
        }

        void renderPass(framebuffer& fbo, texture_handle colortexture, texture_handle depthtexture)
        {
            static id_type gammaId = nameHash("gamma");
            fbo.bind();
            m_aces.bind();
            m_aces.get_uniform<float>(gammaId).set_value(2.2f);
            m_aces.get_uniform_with_location<texture_handle>(SV_SCENECOLOR).set_value(colortexture);
            renderQuad();
            m_aces.release();
            fbo.release();
        }

    };


}
