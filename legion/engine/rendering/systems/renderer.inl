#include "renderer.hpp"
#pragma once

namespace legion::rendering
{
    template<typename PipelineType>
    std::unordered_map<app::window, std::unique_ptr<PipelineType>> pipeline_provider<PipelineType>::m_pipelines;

    template<typename PipelineType>
    inline RenderPipelineBase* pipeline_provider<PipelineType>::get(app::window& context)
    {
        if (!m_pipelines.count(context))
        {
            m_pipelines.emplace(context, std::make_unique<PipelineType>());
            RenderStageBase::m_pipeline = m_pipelines[context].get();
            RenderStageBase::m_pipeline->init(context);
        }
        else
            RenderStageBase::m_pipeline = m_pipelines[context].get();

        return RenderStageBase::m_pipeline;
    }

    template<typename PipelineType>
    inline void pipeline_provider<PipelineType>::shutdown()
    {
        for (auto& [_, pipeline] : m_pipelines)
            pipeline->shutdown();

        m_pipelines.clear();
    }

    template<typename Pipeline CNDOXY(inherits_from<Pipeline, RenderPipeline<Pipeline>>)>
    inline void Renderer::setPipeline()
    {
        OPTICK_EVENT();
        m_pipelineProvider = std::make_unique<pipeline_provider<Pipeline>>();
    }
}
