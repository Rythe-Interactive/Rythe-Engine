#pragma once

#include <application/window/window.hpp>
#include <core/core.hpp>
#include <imgui/backends/imgui_impl_glfw.h>
#include <imgui/backends/imgui_impl_opengl3.h>
#include <atomic>
#include <imgui/imnodes.h>

namespace legion::rendering {

class ImguiGuiSystem : public System<ImguiGuiSystem>
{
public:
    inline static std::atomic_bool initialized = (false);

    void setup() override
    {
        m_scheduler->sendCommand(m_scheduler->getChainThreadId("Rendering"), [](void* param)
                {
                    ImguiGuiSystem* self = reinterpret_cast<ImguiGuiSystem*>(param);
                    log::trace("Waiting on main window.");

                    while (!self->main_window_valid())
                        std::this_thread::yield();

                    app::window window = self->get_main_window();

                    log::trace("Initializing context.");

                    async::readwrite_guard guard(*window.lock);
                    app::ContextHelper::makeContextCurrent(window);


                    auto* ctx = ImGui::CreateContext();
                    ImGui::SetCurrentContext(ctx);
                    ImGui_ImplGlfw_InitForOpenGL(window,true);
                    ImGui_ImplOpenGL3_Init("#version 330");
                    imnodes::Initialize();

                    app::ContextHelper::makeContextCurrent(nullptr);

                    self->initialized.store(true, std::memory_order_release);
                }, this);

            while (!initialized.load(std::memory_order_acquire))
                std::this_thread::yield();
    }

private:

    bool main_window_valid()
    {
        return m_ecs->world.has_component<app::window>();
    }

    app::window get_main_window()
    {
        return m_ecs->world.get_component_handle<app::window>().read();
    }
};

}
