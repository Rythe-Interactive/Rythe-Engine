#pragma once

namespace legion::rendering
{
    template<typename Pipeline, typename ...Args CNDOXY(inherits_from<Pipeline, RenderPipeline<Pipeline>>)>
    inline void Renderer::setPipeline(Args&&... args)
    {
        OPTICK_EVENT();
        m_pipelineProvider = delegate<RenderPipelineBase* (app::window&)>::create([&](app::window& context)
        {
            static std::unordered_map<app::window, std::unique_ptr<Pipeline>> m_pipelines;

            if (!m_pipelines.count(context))
            {
                m_pipelines.emplace(context, std::unique_ptr<Pipeline>(new Pipeline(std::forward(args)...)));
                RenderStageBase::m_pipeline = m_pipelines[context].get();
                m_pipelines[context]->init(context);
            }
            else
                RenderStageBase::m_pipeline = m_pipelines[context].get();

            return RenderStageBase::m_pipeline;
        });
    }
}
