#include <rendering/pipeline/base/pipeline.hpp>

namespace legion::rendering
{
    framebuffer RenderPipelineBase::addFramebuffer(const std::string& name, GLenum target)
    {
        id_type id = nameHash(name);

        if (m_framebuffers.contains(id))
        {
            if (m_framebuffers[id].target == target)
                return m_framebuffers[id];
            else
                return framebuffer();
        }

        m_framebuffers[id] = framebuffer(target);

        return m_framebuffers[id];
    }

    framebuffer RenderPipelineBase::getFramebuffer(const std::string& name)
    {
        return framebuffer();
    }

}
