#pragma once
#include <rendering/data/model.hpp>
#include <rendering/data/shader.hpp>
#include <rendering/components/renderable.hpp>
#include <rendering/components/light.hpp>
#include <rendering/components/camera.hpp>
#include <rendering/debugrendering.hpp>
#include <rendering/pipeline/base/pipeline.hpp>

#include <unordered_set>

namespace legion::rendering
{
    class Renderer final : public System<Renderer>
    {
    private:
        static std::unique_ptr<RenderPipelineBase> m_pipeline;

    public:
        virtual void setup();

        void render(time::span deltatime);

        template<typename Pipeline, typename... Args, inherits_from<Pipeline, RenderPipeline<Pipeline>> = 0>
        static void SetPipeline(Args&&... args);
    };
}

#include <rendering/systems/newrenderer.inl>
