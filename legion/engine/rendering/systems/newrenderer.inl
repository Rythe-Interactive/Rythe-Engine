#pragma once

namespace legion::rendering
{
    template<typename Pipeline, typename ...Args, inherits_from<Pipeline, RenderPipeline<Pipeline>>>
    inline void Renderer::SetPipeline(Args&&... args)
    {
        m_pipeline = std::unique_ptr<RenderPipelineBase>(new Pipeline(std::forward(args)...));
        RenderStage::m_pipeline = m_pipeline.get();
    }
}
