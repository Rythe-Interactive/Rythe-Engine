#pragma once
#include <rendering/pipeline/base/renderstage.hpp>
#include <rendering/pipeline/base/pipeline.hpp>

namespace legion::rendering
{
    class SubmitStage : public RenderStage<SubmitStage>
    {
        unsigned int m_quadVAO, m_quadVBO;
        shader_handle m_screenShader;

    public:
        virtual void setup(app::window& context) override;
        virtual void render(app::window& context, camera& cam, const camera::camera_input& camInput, time::span deltaTime) override;
        virtual priority_type priority() override;
    };
}
