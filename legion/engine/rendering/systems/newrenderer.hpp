#pragma once
#include <rendering/pipeline/base/pipeline.hpp>

#include <unordered_map>

namespace legion::rendering
{
    class Renderer final : public System<Renderer>
    {
    private:
        static delegate<RenderPipelineBase*(app::window&)> m_pipelineProvider;
        std::atomic_bool initialized = false;

        static void debugCallback(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length, const GLchar* message, const void* userParam);
        bool initContext(const app::window& window);

    public:
        virtual void setup();

        void render(time::span deltatime);

        template<typename Pipeline, typename... Args, inherits_from<Pipeline, RenderPipeline<Pipeline>> = 0>
        static void setPipeline(Args&&... args);
    };
}

#include <rendering/systems/newrenderer.inl>
