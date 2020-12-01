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

        template<typename T, typename... Args>
        T* create_meta(std::string name, Args&&... args);

        template<typename T>
        T* get_meta(std::string name);

        virtual void init() LEGION_PURE;

        virtual void render() LEGION_PURE;
    };

    template<typename Self>
    class RenderPipeline : public RenderPipelineBase
    {
    protected:
        static std::multimap<priority_type, std::unique_ptr<RenderStage>, std::greater<>> m_stages;

    public:
        template<typename StageType, priority_type priority = default_priority, inherits_from<StageType, RenderStage> = 0>
        static void attachStage();

        template<typename StageType, inherits_from<StageType, RenderStage> = 0>
        static void attachStage(priority_type priority = default_priority);

        static void attachStage(std::unique_ptr<RenderStage>&& stage, priority_type priority = default_priority);

        virtual void setup() LEGION_PURE;

        void init() override;
        void render(app::window context, camera cam) override;
    };
}

#include <rendering/pipeline/base/pipeline.inl>
