#pragma once
#include <application/application.hpp>
#include <rendering/components/camera.hpp>
#include <rendering/pipeline/base/pipelinebase.hpp>

#define setup_priority 64
#define opaque_priority 32
//      default_priority 0
#define transparent_priority -16
#define volumetric_priority -32
#define post_fx_priority -48
#define ui_priority -64
#define submit_priority -96

namespace legion::rendering
{
    class RenderPipelineBase;

    class RenderStageBase
    {
        friend class Renderer;
    private:
        bool m_isInitialized = false;

    protected:
        virtual void setup(app::window& context) LEGION_PURE;

    public:
        static RenderPipelineBase* m_pipeline;

        inline bool isInitialized() { return m_isInitialized; }

        inline void init(app::window& context)
        {
            OPTICK_EVENT("Setup render stage");
            m_isInitialized = true;
            setup(context);
        }

        virtual void render(app::window& context, camera& cam, const camera::camera_input& camInput, time::span deltaTime) LEGION_PURE;
        virtual priority_type priority() LEGION_IMPURE_RETURN(default_priority);

    protected:
        void abort();

        template<typename T>
        L_NODISCARD bool has_meta(const std::string& name);

        template<typename T, typename... Args>
        T* create_meta(const std::string& name, Args&&... args);

        template<typename T>
        L_NODISCARD T* get_meta(const std::string& name);

        template<typename T>
        L_NODISCARD bool has_meta(id_type nameHash);

        template<typename T, typename... Args>
        T* create_meta(id_type nameHash, Args&&... args);

        template<typename T>
        L_NODISCARD T* get_meta(id_type nameHash);

        framebuffer* addFramebuffer(const std::string& name, GLenum target = GL_FRAMEBUFFER);
        L_NODISCARD bool hasFramebuffer(const std::string& name, GLenum target = GL_FRAMEBUFFER);
        L_NODISCARD framebuffer* getFramebuffer(const std::string& name);
        framebuffer* addFramebuffer(id_type nameHash, GLenum target = GL_FRAMEBUFFER);
        L_NODISCARD bool hasFramebuffer(id_type nameHash, GLenum target = GL_FRAMEBUFFER);
        L_NODISCARD framebuffer* getFramebuffer(id_type nameHash);
    };

    template<typename SelfType>
    class RenderStage : public RenderStageBase, protected System<SelfType>
    {
    };
}

#include <rendering/pipeline/base/renderstage.inl>
