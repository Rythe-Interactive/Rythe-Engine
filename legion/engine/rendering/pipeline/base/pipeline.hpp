#pragma once
#include <rendering/data/framebuffer.hpp>
#include <rendering/components/camera.hpp>
#include <rendering/pipeline/base/renderstage.hpp>
#include <application/application.hpp>

#include <memory>
#include <any>

namespace legion::rendering
{
    class RenderPipelineBase
    {
        friend class Renderer;
    protected:
        sparse_map<id_type, framebuffer> m_framebuffers;
        sparse_map<id_type, std::any> m_metadata;

        static ecs::EcsRegistry* m_ecs;
        static schd::Scheduler* m_scheduler;
        static events::EventBus* m_eventBus;
    public:

        template<typename T>
        bool has_meta(const std::string& name);

        template<typename T, typename... Args>
        T* create_meta(const std::string& name, Args&&... args);

        template<typename T>
        T* get_meta(const std::string& name);

        template<typename T>
        bool has_meta(id_type nameHash);

        template<typename T, typename... Args>
        T* create_meta(id_type nameHash, Args&&... args);

        template<typename T>
        T* get_meta(id_type nameHash);

        framebuffer addFramebuffer(const std::string& name, GLenum target = GL_FRAMEBUFFER);
        bool hasFramebuffer(const std::string& name, GLenum target = GL_FRAMEBUFFER);
        framebuffer getFramebuffer(const std::string& name);
        framebuffer addFramebuffer(id_type nameHash, GLenum target = GL_FRAMEBUFFER);
        bool hasFramebuffer(id_type nameHash, GLenum target = GL_FRAMEBUFFER);
        framebuffer getFramebuffer(id_type nameHash);

        virtual void init(app::window& context) LEGION_PURE;

        virtual void render(app::window& context, camera& cam, const camera::camera_input& camInput, time::span deltaTime) LEGION_PURE;
    };

    template<typename Self>
    class RenderPipeline : public RenderPipelineBase
    {
        friend class Renderer;
    protected:
        static std::multimap<priority_type, std::unique_ptr<RenderStageBase>, std::greater<>> m_stages;

    public:
        template<typename StageType, inherits_from<StageType, RenderStage<StageType>> = 0>
        static void attachStage();

        static void attachStage(std::unique_ptr<RenderStageBase>&& stage);

        virtual void setup(app::window& context) LEGION_PURE;

        void init(app::window& context) override;
        void render(app::window& context, camera& cam, const camera::camera_input& camInput, time::span deltaTime) override;
    };
}

#include <rendering/pipeline/base/pipeline.inl>
