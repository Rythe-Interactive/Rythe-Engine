#include <rendering/systems/newrenderer.hpp>

namespace legion::rendering
{
    void Renderer::setup()
    {
        RenderPipelineBase::m_ecs = m_ecs;
        RenderPipelineBase::m_scheduler = m_scheduler;
        RenderPipelineBase::m_eventBus = m_eventBus;
        
        RenderStage::m_ecs = m_ecs;
        RenderStage::m_scheduler = m_scheduler;
        RenderStage::m_eventBus = m_eventBus;

        m_pipeline->init();

        createProcess<&Renderer::render>("Rendering");
    }

    void Renderer::render(time::span deltatime)
    {
        static auto cameraQuery = createQuery<camera>();
        for (auto ent : cameraQuery)
        {
            auto cam = ent.get_component_handle<camera>().read();
            app::window win = cam.m_targetWindow.read();
            if (!win)
                win = m_ecs->world.get_component_handle<app::window>().read();
            if (!win)
                return;

            m_pipeline->render(win, cam, deltatime);
        }
    }
}
