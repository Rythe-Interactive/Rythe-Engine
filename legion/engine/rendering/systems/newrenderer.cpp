#include <rendering/systems/newrenderer.hpp>

namespace legion::rendering
{
    void Renderer::setup()
    {
        RenderPipelineBase::m_ecs = m_ecs;
        RenderPipelineBase::m_scheduler = m_scheduler;
        RenderPipelineBase::m_eventBus = m_eventBus;
        
        RenderStageBase::m_ecs = m_ecs;
        RenderStageBase::m_scheduler = m_scheduler;
        RenderStageBase::m_eventBus = m_eventBus;

        createProcess<&Renderer::render>("Rendering");
    }

    void Renderer::render(time::span deltatime)
    {
        static auto cameraQuery = createQuery<camera>();
        for (auto ent : cameraQuery)
        {
            auto cam = ent.get_component_handle<camera>().read();
            app::window win = cam.targetWindow.read();
            if (!win)
                win = m_ecs->world.get_component_handle<app::window>().read();
            if (!win)
                continue;

            auto viewportSize = win.viewportSize();

            if (viewportSize.x == 0 || viewportSize.y == 0)
                continue;

            position camPos = ent.get_component_handle<position>().read();
            rotation camRot = ent.get_component_handle<rotation>().read();
            scale camScale = ent.get_component_handle<scale>().read();

            math::mat4 view(1.f);
            math::compose(view, camScale, camRot, camPos);
            view = math::inverse(view);

            math::mat4 projection = cam.get_projection(((float)viewportSize.x) / viewportSize.y);

            camera::camera_input cam_input_data(view, projection, camPos, 0, camRot.forward());

            m_pipelineProvider(win)->render(win, cam, cam_input_data, deltatime);
        }
    }
}
