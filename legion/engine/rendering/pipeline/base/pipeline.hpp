#pragma once
#include <rendering/data/model.hpp>
#include <rendering/data/shader.hpp>
#include <rendering/data/framebuffer.hpp>
#include <rendering/components/renderable.hpp>
#include <rendering/components/camera.hpp>
#include <rendering/pipeline/base/renderstage.hpp>

#include <map>
#include <memory>
#include <any>

namespace legion::rendering
{
    class RenderPipelineBase
    {
    protected:
        sparse_map<id_type, framebuffer> m_framebuffers;
        sparse_map<id_type, std::any> m_metadata;

    public:

        template<typename T>
        bool has_meta(std::string name);

        template<typename T>
        T* create_meta(std::string name);

        template<typename T>
        T* get_meta(std::string name);

        virtual void setup() LEGION_PURE;

        virtual void render() LEGION_PURE;
    };

    template<typename Self>
    class RenderPipeline : public RenderPipelineBase
    {
    protected:
        static std::multimap<priority_type, std::unique_ptr<RenderStage>, std::greater<>> m_stages;

    public:
        template<typename StageType, priority_type priority = default_priority, inherits_from<StageType, RenderStage> = 0>
        static void attachStage()
        {
            m_stages.emplace({ priority, std::unique_ptr<RenderStage>(new StageType()) });
        }

        template<typename StageType, inherits_from<StageType, RenderStage> = 0>
        static void attachStage(priority_type priority = default_priority)
        {
            m_stages.emplace({ priority, std::unique_ptr<RenderStage>(new StageType()) });
        }

        static void attachStage(std::unique_ptr<RenderStage>&& stage, priority_type priority = default_priority);

        void render(app::window context, camera cam) override;
    };

    template<typename T>
    inline bool RenderPipelineBase::has_meta(std::string name)
    {
        id_type id = nameHash(name);
        return m_metadata.count(id) && (m_metadata[id].first == typeHash<T>());
    }

    template<typename T>
    inline T* RenderPipelineBase::create_meta(std::string name)
    {
        id_type id = nameHash(name);
        id_type typeId = typeHash<T>();

        if (m_metadata.count(id))
        {
            if (m_metadata[id].first == typeId)
                return reinterpret_cast<T*>(m_metadata[id].second.get());
            else
                return nullptr;
        }

        T* ptr = new T();
        m_metadata.emplace(id, { typeId, std::unique_ptr<void>(ptr) });
        return ptr;
    }

    template<typename T>
    inline T* RenderPipelineBase::get_meta(std::string name)
    {
        id_type id = nameHash(name);
        id_type typeId = typeHash<T>();

        if(m_metadata.count(id) && (m_metadata[id].first == typeId)
            return reinterpret_cast<T*>(m_metadata[id].second.get());
        return nullptr;
    }

}
