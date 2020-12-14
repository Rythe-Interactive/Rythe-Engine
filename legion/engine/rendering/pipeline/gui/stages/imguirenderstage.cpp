#include <imgui/imnodes.h>
#include <imgui/backends/imgui_impl_glfw.h>
#include <imgui/backends/imgui_impl_opengl3.h>
#include <rendering/pipeline/gui/stages/imguirenderstage.hpp>

namespace legion::rendering
{
    static bool initialized = false;
    void ImGuiStage::setup(app::window& context)
    {

        //make context current
        application::context_guard guard(context);

        //init imgui
        auto* ctx = ImGui::CreateContext();
        ImGui::SetCurrentContext(ctx);
        ImGui_ImplGlfw_InitForOpenGL(context, true);
        ImGui_ImplOpenGL3_Init("#version 330");

        //init imnodes
        imgui::nodes::Initialize();
        initialized = true;
    }

    void ImGuiStage::render(app::window& context, camera& cam, const camera::camera_input& camInput,
        time::span deltaTime)
    {
        if (!initialized) //FIXME(algorythmix): Workaround for #243 
        {
            //make context current
            application::context_guard guard(context);

            //init imgui
            auto* ctx = ImGui::CreateContext();
            ImGui::SetCurrentContext(ctx);
            ImGui_ImplGlfw_InitForOpenGL(context, true);
            ImGui_ImplOpenGL3_Init("#version 330");

            //init imnodes
            imgui::nodes::Initialize();
            initialized = true;
        }
        //make context current
        application::context_guard guard(context);

        //start imgui frame
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        //call gui callback
        if (!OnGuiRender.isNull())
        {
            OnGuiRender();
        }

        //render gui
        ImGui::Render();
        auto* draw_data = ImGui::GetDrawData();
        ImGui_ImplOpenGL3_RenderDrawData(draw_data);
    }

    priority_type ImGuiStage::priority()
    {
        return -100;
    }

    multicast_delegate<void()> ImGuiStage::OnGuiRender;
}
