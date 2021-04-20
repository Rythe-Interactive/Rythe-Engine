#include <rendering/pipeline/base/pipelinebase.hpp>

namespace legion::rendering
{
    std::atomic_bool RenderPipelineBase::m_exiting = { false };

    void RenderPipelineBase::exit()
    {
        m_exiting.store(true, std::memory_order_release);
    }

    void RenderPipelineBase::abort()
    {
        m_abort = true;
    }

    framebuffer* RenderPipelineBase::addFramebuffer(const std::string& name, GLenum target)
    {
        OPTICK_EVENT();
        id_type id = nameHash(name);

        if (m_framebuffers.contains(id))
        {
            if (m_framebuffers[id].target() == target)
                return &m_framebuffers[id];
            else
                return nullptr;
        }

        m_framebuffers[id] = framebuffer(target);

        return &m_framebuffers[id];
    }

    L_NODISCARD bool RenderPipelineBase::hasFramebuffer(const std::string& name, GLenum target)
    {
        OPTICK_EVENT();
        id_type id = nameHash(name);
        return m_framebuffers.contains(id) && m_framebuffers[id].target() == target;
    }

    L_NODISCARD framebuffer* RenderPipelineBase::getFramebuffer(const std::string& name)
    {
        OPTICK_EVENT();
        id_type id = nameHash(name);
        if(m_framebuffers.contains(id))
            return &m_framebuffers[id];
        return nullptr;
    }

    framebuffer* RenderPipelineBase::addFramebuffer(id_type nameHash, GLenum target)
    {
        OPTICK_EVENT();
        if (m_framebuffers.contains(nameHash))
        {
            if (m_framebuffers[nameHash].target() == target)
                return &m_framebuffers[nameHash];
            else
                return nullptr;
        }

        m_framebuffers[nameHash] = framebuffer(target);

        return &m_framebuffers[nameHash];
    }

    L_NODISCARD bool RenderPipelineBase::hasFramebuffer(id_type nameHash, GLenum target)
    {
        OPTICK_EVENT();
        return m_framebuffers.contains(nameHash) && m_framebuffers[nameHash].target() == target;
    }

    L_NODISCARD framebuffer* RenderPipelineBase::getFramebuffer(id_type nameHash)
    {
        OPTICK_EVENT();
        if (m_framebuffers.contains(nameHash))
            return &m_framebuffers[nameHash];
        return nullptr;
    }

}
