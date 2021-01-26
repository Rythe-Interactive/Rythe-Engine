#pragma once
#include <rendering/data/vertexarray.hpp>
#include <rendering/data/buffer.hpp>


#include <rendering/data/framebuffer.hpp>
#include <rendering/data/shader.hpp>

#include <rendering/data/screen_quad.hpp>

#include <rendering/pipeline/base/pipelinebase.hpp>

namespace legion::rendering
{
    class PostProcessingEffectBase
    {
    public:
        std::vector<delegate<void(framebuffer&, RenderPipelineBase*, camera&, const camera::camera_input&, time::span)>> renderPasses;
        virtual id_type getId() const LEGION_PURE;
        virtual const std::string& getName() const LEGION_PURE;
        void init(app::window& context)
        {
            m_initialized = true;
            m_quad = screen_quad(nullptr);
            setup(context);
        }

        bool isInitialized() const { return m_initialized; }

    protected:
        virtual void setup(app::window& context) LEGION_PURE;
        void renderQuad()
        {
            OPTICK_EVENT();
            m_quad.render();
        }
    private:
        bool m_initialized = false;
        screen_quad m_quad;
    };

    struct framebuffer;
    /**
     * @class PostProcessingEffect
     * @brief A base class for post processing effect.
     */

    template<typename Self>
    class PostProcessingEffect : public PostProcessingEffectBase
    {
    public:
        virtual id_type getId() const override { return id; }
        virtual const std::string& getName() const override { return name; }
        static const id_type id;
        static const std::string name;

    protected:
        template<void(Self::* func_type)(framebuffer&, RenderPipelineBase* ,camera&, const camera::camera_input&, time::span)>
        void addRenderPass()
        {
            renderPasses.push_back(delegate<void(framebuffer&, RenderPipelineBase*, camera&, const camera::camera_input&, time::span)>::create<Self, func_type>(reinterpret_cast<Self*>(this)));
        }
    };

    template<typename Self>
    const id_type PostProcessingEffect<Self>::id = typeHash<Self>();

    template<typename Self>
    const std::string PostProcessingEffect<Self>::name = nameOfType<Self>();

}
