#include <imgui/imnodes.h>
#include <imgui/backends/imgui_impl_glfw.h>
#include <imgui/backends/imgui_impl_opengl3.h>
#include <rendering/pipeline/gui/stages/imguirenderstage.hpp>

namespace legion::rendering
{
    void ImGuiStage::setup(app::window& context)
    {
        OPTICK_EVENT();
        //make context current
        application::context_guard guard(context);

        //init imgui
        auto* ctx = ImGui::CreateContext();
        ImGui::SetCurrentContext(ctx);
        ImGui_ImplGlfw_InitForOpenGL(context, true);
        ImGui_ImplOpenGL3_Init("#version 460");

        //init imnodes
        imgui::nodes::Initialize();
    }

    void ImGuiStage::render(app::window& context, camera& cam, const camera::camera_input& camInput,
        time::span deltaTime)
    {
        OPTICK_EVENT();
        static id_type mainId = nameHash("main");

        auto fbo = getFramebuffer(mainId);
        if (!fbo)
        {
            log::error("Main frame buffer is missing.");
            abort();
            return;
        }

        //make context current
        application::context_guard guard(context);
        if (!guard.contextIsValid())
        {
            abort();
            return;
        }

        auto [valid, message] = fbo->verify();
        if (!valid)
        {
            log::error("Main frame buffer isn't complete: {}", message);
            abort();
            return;
        }

        fbo->bind();

        //start imgui frame
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        //call gui callback
        if (!m_onGuiRender.isNull())
        {
            m_onGuiRender(context, cam, camInput, deltaTime);
        }

        //render gui
        ImGui::Render();
        auto* draw_data = ImGui::GetDrawData();
        ImGui_ImplOpenGL3_RenderDrawData(draw_data);

        fbo->release();
    }

    priority_type ImGuiStage::priority()
    {
        return ui_priority;
    }

    multicast_delegate<void(app::window&, camera&, const camera::camera_input&, time::span)> ImGuiStage::m_onGuiRender;
}
