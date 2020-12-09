#include <rendering/pipeline/base/pipeline.hpp>

namespace legion::rendering
{
    ecs::EcsRegistry* RenderPipelineBase::m_ecs;
    schd::Scheduler* RenderPipelineBase::m_scheduler;
    events::EventBus* RenderPipelineBase::m_eventBus;

    void RenderPipelineBase::abort()
    {
        m_abort = true;
    }

    framebuffer RenderPipelineBase::addFramebuffer(const std::string& name, GLenum target)
    {
        id_type id = nameHash(name);

        if (m_framebuffers.contains(id))
        {
            if (m_framebuffers[id].target() == target)
                return m_framebuffers[id];
            else
                return framebuffer();
        }

        m_framebuffers[id] = framebuffer(target);

        return m_framebuffers[id];
    }

    bool RenderPipelineBase::hasFramebuffer(const std::string& name, GLenum target)
    {
        id_type id = nameHash(name);
        return m_framebuffers.contains(id) && m_framebuffers[id].target() == target;
    }

    framebuffer RenderPipelineBase::getFramebuffer(const std::string& name)
    {
        id_type id = nameHash(name);
        if(m_framebuffers.contains(id))
            return m_framebuffers[id];
        return framebuffer();
    }

    framebuffer RenderPipelineBase::addFramebuffer(id_type nameHash, GLenum target)
    {
        if (m_framebuffers.contains(nameHash))
        {
            if (m_framebuffers[nameHash].target() == target)
                return m_framebuffers[nameHash];
            else
                return framebuffer();
        }

        m_framebuffers[nameHash] = framebuffer(target);

        return m_framebuffers[nameHash];
    }

    bool RenderPipelineBase::hasFramebuffer(id_type nameHash, GLenum target)
    {
        return m_framebuffers.contains(nameHash) && m_framebuffers[nameHash].target() == target;
    }

    framebuffer RenderPipelineBase::getFramebuffer(id_type nameHash)
    {
        if (m_framebuffers.contains(nameHash))
            return m_framebuffers[nameHash];
        return framebuffer();
    }

}
