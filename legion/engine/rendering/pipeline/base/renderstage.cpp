#include <rendering/pipeline/base/renderstage.hpp>
#include <rendering/pipeline/base/pipeline.hpp>

namespace legion::rendering
{
    ecs::EcsRegistry* RenderStageBase::m_ecs;
    schd::Scheduler* RenderStageBase::m_scheduler;
    events::EventBus* RenderStageBase::m_eventBus;
    RenderPipelineBase* RenderStageBase::m_pipeline;

    framebuffer RenderStageBase::addFramebuffer(const std::string& name, GLenum target)
    {
        return m_pipeline->addFramebuffer(name, target);
    }

    bool RenderStageBase::hasFramebuffer(const std::string& name, GLenum target)
    {
        return m_pipeline->hasFramebuffer(name, target);
    }

    framebuffer RenderStageBase::getFramebuffer(const std::string& name)
    {
        return m_pipeline->getFramebuffer(name);
    }

    framebuffer RenderStageBase::addFramebuffer(id_type nameHash, GLenum target)
    {
        return m_pipeline->addFramebuffer(nameHash, target);
    }

    bool RenderStageBase::hasFramebuffer(id_type nameHash, GLenum target)
    {
        return m_pipeline->hasFramebuffer(nameHash, target);
    }

    framebuffer RenderStageBase::getFramebuffer(id_type nameHash)
    {
        return m_pipeline->getFramebuffer(nameHash);
    }

}
