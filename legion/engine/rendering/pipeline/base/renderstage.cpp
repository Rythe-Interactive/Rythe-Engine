#include <rendering/pipeline/base/renderstage.hpp>

namespace legion::rendering
{
    ecs::EcsRegistry* RenderStage::m_ecs;
    schd::Scheduler* RenderStage::m_scheduler;
    events::EventBus* RenderStage::m_eventBus;
    RenderPipelineBase* RenderStage::m_pipeline;
}
