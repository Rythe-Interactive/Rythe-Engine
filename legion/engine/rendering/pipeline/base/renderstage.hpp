#pragma once
#include <application/application.hpp>
#include <rendering/components/camera.hpp>

namespace legion::rendering
{
    class RenderPipelineBase;

    class RenderStage
    {
        friend class RenderPipeline;
    protected:
        static ecs::EcsRegistry* m_ecs;
        static schd::Scheduler* m_scheduler;
        static ecs::QueryRegistry* m_queryRegistry;
        static RenderPipelineBase* m_pipeline;

    public:
        virtual void render(app::window context, camera cam) LEGION_PURE;
    };
}
