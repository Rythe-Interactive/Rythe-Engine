#pragma once
#include <rendering/pipeline/base/pipeline.hpp>
#include <rendering/pipeline/default/defaultpipeline.hpp>

#include <unordered_map>

namespace legion::rendering
{
    struct pipeline_provider_base
    {
        virtual RenderPipelineBase* get(app::window& context) LEGION_PURE;
        virtual void shutdown() LEGION_PURE;
    };

    template<typename PipelineType>
    struct pipeline_provider : public pipeline_provider_base
    {
        static std::unordered_map<app::window, std::unique_ptr<PipelineType>> m_pipelines;

        RenderPipelineBase* get(app::window& context) override;

        void shutdown() override;
    };

    class Renderer final : public System<Renderer>
    {
    private:
        static std::unique_ptr<pipeline_provider_base> m_pipelineProvider;
        std::atomic_bool m_exiting = false;

        static void debugCallback(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length, const GLchar* message, L_MAYBEUNUSED const void* userParam);
        static void debugCallbackARB(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length, const GLchar* message, L_MAYBEUNUSED const void* userParam);
        static void debugCallbackAMD(GLuint id, GLenum category, GLenum severity, GLsizei length, const GLchar* message, L_MAYBEUNUSED void* userParam);
        bool initContext(const app::window& window);

        void setThreadPriority();

        static RenderPipelineBase* m_currentPipeline;

    public:
        Renderer() : System<Renderer>()
        {
            setPipeline<DefaultPipeline>();
        }

        void setup();

        void shutdown();

        void onExit(events::exit& event);

        void render(time::span deltatime);

        template<typename Pipeline CNDOXY(inherits_from<Pipeline, RenderPipeline<Pipeline>> = 0)>
        static void setPipeline();

        L_NODISCARD static RenderPipelineBase* getPipeline(app::window& context);
        L_NODISCARD static RenderPipelineBase* getCurrentPipeline();
        L_NODISCARD static RenderPipelineBase* getMainPipeline();
    };
}

#include <rendering/systems/renderer.inl>
