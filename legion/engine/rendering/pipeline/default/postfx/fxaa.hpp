#pragma once
#include <rendering/data/postprocessingeffect.hpp>
#include <rendering/util/bindings.hpp>

namespace legion::rendering
{
    class FXAA : public PostProcessingEffect<FXAA>
    {
    private:
        shader_handle m_fxaaShader;
    public:
        void setup(app::window& context) override;

        void renderPass(framebuffer& fbo, RenderPipelineBase* pipeline, camera& cam, const camera::camera_input& camInput, time::span deltaTime);
    };


}
