#pragma once
#include <rendering/data/postprocessingeffect.hpp>
#include <rendering/util/bindings.hpp>

namespace legion::rendering
{
    enum struct tonemapping_type
    {
        aces, reinhard, reinhard_jodie, legion, unreal3
    };

    class Tonemapping : public PostProcessingEffect<Tonemapping>
    {
    private:
        static std::atomic<id_type> m_currentShader;
        float exposure;

    public:
        static void setAlgorithm(tonemapping_type type);

        void setup(app::window& context) override;

        void renderPass(framebuffer& fbo, RenderPipelineBase* pipeline, camera& cam, const camera::camera_input& camInput, time::span deltaTime);

    };


}
