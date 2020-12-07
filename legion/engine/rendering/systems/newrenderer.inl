#pragma once

namespace legion::rendering
{
    template<typename Pipeline, typename ...Args, inherits_from<Pipeline, RenderPipeline<Pipeline>>>
    inline void Renderer::setPipeline(Args&&... args)
    {
        m_pipelineProvider = delegate<RenderPipelineBase* (app::window&)>::create([&](app:window& context)
        {
            static std::unordered_map<app:window, std::unique_ptr<Pipeline>> m_pipelines;

            if (!m_pipelines.count(context))
            {
                m_pipelines.emplace(context, std::unique_ptr<Pipeline>(new Pipeline(std::forward(args)...)));
                m_pipelines[context]->init(context);
            }

            RenderStageBase::m_pipeline = m_pipelines[context].get();
            return RenderStageBase::m_pipeline;
        });
    }
}
