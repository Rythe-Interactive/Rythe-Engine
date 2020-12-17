#pragma once

namespace legion::rendering
{
    template<typename T>
    L_NODISCARD inline bool RenderStageBase::has_meta(const std::string& name)
    {
        return m_pipeline->has_meta<T>(name);
    }

    template<typename T, typename... Args>
    inline T* RenderStageBase::create_meta(const std::string& name, Args&&... args)
    {
        return m_pipeline->create_meta<T>(name, std::forward<Args>(args)...);
    }

    template<typename T>
    L_NODISCARD inline T* RenderStageBase::get_meta(const std::string& name)
    {
        return m_pipeline->get_meta<T>(name);
    }

    template<typename T>
    L_NODISCARD inline bool RenderStageBase::has_meta(id_type nameHash)
    {
        return m_pipeline->has_meta<T>(nameHash);
    }

    template<typename T, typename ...Args>
    inline T* RenderStageBase::create_meta(id_type nameHash, Args && ...args)
    {
        return m_pipeline->create_meta<T>(nameHash, std::forward<Args>(args)...);
    }

    template<typename T>
    L_NODISCARD inline T* RenderStageBase::get_meta(id_type nameHash)
    {
        return m_pipeline->get_meta<T>(nameHash);
    }

}
