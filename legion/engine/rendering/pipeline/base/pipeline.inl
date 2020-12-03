#include "pipeline.hpp"
#pragma once

namespace legion::rendering
{
    template<typename T>
    inline bool RenderPipelineBase::has_meta(std::string name)
    {
        id_type id = nameHash(name);
        return m_metadata.count(id) && (m_metadata[id].type() == typeid(T));
    }

    template<typename T, typename... Args>
    inline T* RenderPipelineBase::create_meta(std::string name, Args&&... args)
    {
        id_type id = nameHash(name);

        if (m_metadata.count(id))
        {
            if (m_metadata[id].type() == typeid(T))
                return std::any_cast<T>(&m_metadata[id]);
            else
                return nullptr;
        }

        m_metadata.emplace(id, std::make_any<T>(std::forward(args)...);
        return std::any_cast<T>(&m_metadata[id]);
    }

    template<typename T>
    inline T* RenderPipelineBase::get_meta(std::string name)
    {
        id_type id = nameHash(name);

        if (m_metadata.count(id) && (m_metadata[id].type() == typeid(T))
            return std::any_cast<T>(&m_metadata[id]);
            return nullptr;
    }

    template<typename Self>
    template<typename StageType, inherits_from<StageType, RenderStage>>
    inline void RenderPipeline<Self>::attachStage()
    {
        auto ptr = new StageType();
        m_stages.emplace(ptr->priority(), std::unique_ptr<RenderStage>(ptr));
    }

    template<typename Self>
    inline void RenderPipeline<Self>::attachStage(std::unique_ptr<RenderStage>&& stage)
    {
        m_stages.emplace(stage->priority(), std::forward(stage));
    }

    template<typename Self>
    inline void RenderPipeline<Self>::init()
    {
        setup();
        for (auto& [_, stage] : m_stages)
            stage->setup();
    }

    template<typename Self>
    inline void RenderPipeline<Self>::render(app::window& context, camera& cam, time::span deltaTime)
    {
        for (auto& [_, stage] : m_stages)
            stage->render(context, cam, deltaTime);
    }
}
