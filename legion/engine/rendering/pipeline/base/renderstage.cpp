#include <rendering/pipeline/base/renderstage.hpp>
#include <rendering/pipeline/base/pipeline.hpp>

namespace legion::rendering
{
    RenderPipelineBase* RenderStageBase::m_pipeline;

    void RenderStageBase::abort()
    {
        m_pipeline->abort();
    }

    framebuffer* RenderStageBase::addFramebuffer(const std::string& name, GLenum target)
    {
        OPTICK_EVENT();
        return m_pipeline->addFramebuffer(name, target);
    }

    L_NODISCARD bool RenderStageBase::hasFramebuffer(const std::string& name, GLenum target)
    {
        OPTICK_EVENT();
        return m_pipeline->hasFramebuffer(name, target);
    }

    L_NODISCARD framebuffer* RenderStageBase::getFramebuffer(const std::string& name)
    {
        OPTICK_EVENT();
        return m_pipeline->getFramebuffer(name);
    }

    framebuffer* RenderStageBase::addFramebuffer(id_type nameHash, GLenum target)
    {
        OPTICK_EVENT();
        return m_pipeline->addFramebuffer(nameHash, target);
    }

    L_NODISCARD bool RenderStageBase::hasFramebuffer(id_type nameHash, GLenum target)
    {
        OPTICK_EVENT();
        return m_pipeline->hasFramebuffer(nameHash, target);
    }

    L_NODISCARD framebuffer* RenderStageBase::getFramebuffer(id_type nameHash)
    {
        OPTICK_EVENT();
        return m_pipeline->getFramebuffer(nameHash);
    }

}
