#pragma once
#include <core/core.hpp>
#include <imgui/imgui.h>
#include <rendering/pipeline/gui/stages/imguirenderstage.hpp>

class GuiTestSystem : public System<GuiTestSystem>
{
    void setup() override
    {

        static_cast<DefaultPipeline*>(Renderer::getMainPipeline())->attachStage<ImGuiStage>();

        //gui code goes here
        ImGuiStage::OnGuiRender += [this]()
        {
             ImGui::ShowDemoWindow();
        };
    }
};
