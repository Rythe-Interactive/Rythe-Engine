#pragma once
#include <rendering/data/vertexarray.hpp>
#include <rendering/data/buffer.hpp>


#include <rendering/data/framebuffer.hpp>
#include <rendering/data/shader.hpp>

#include <rendering/data/screen_quad.hpp>

namespace legion::rendering
{
    class PostProcessingEffectBase
    {
    public:
        std::vector<delegate<void(framebuffer&, texture_handle, texture_handle, time::span)>> renderPasses;
        virtual id_type getId() const LEGION_PURE;
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
        virtual id_type getId() const override { return m_id; }
        static const id_type m_id;

    protected:
        template<void(Self::* func_type)(framebuffer&, texture_handle, texture_handle, time::span)>
        void addRenderPass()
        {
            renderPasses.push_back(delegate<void(framebuffer&, texture_handle, texture_handle, time::span)>::create<Self, func_type>(reinterpret_cast<Self*>(this)));
        }
    };

    template<typename Self>
    const id_type PostProcessingEffect<Self>::m_id = typeHash<Self>();

}
