#pragma once
#include <application/window/window.hpp>
#include <application/context/contexthelper.hpp>

/**@file windowsystem.hpp
*/

namespace args::application
{
    class WindowSystem final : public System<WindowSystem>
    {
    private:
        inline static sparse_map<GLFWwindow*, ecs::component_handle<window>> m_windowComponents;

        static void closeWindow(GLFWwindow* win)
        {
            ContextHelper::destroyWindow(win);
            auto handle = m_windowComponents[win];
            if (handle.valid())
            {
                handle.write(nullptr, std::memory_order_relaxed);
                handle.destroy();
                m_windowComponents.erase(win);
            }
        }

    public:
        virtual void setup()
        {
            ContextHelper::init();
            ContextHelper::swapInterval(1);

            auto handle = m_ecs->world.add_component<window_request>();

            createProcess<&WindowSystem::updateWindows>("Rendering");
        }

        void createWindows()
        {
            static auto query = createQuery<window_request>();

            for (auto entity : query)
            {
                window_request request = entity.get_component<window_request>().read();
                entity.remove_component<window_request>();
                auto handle = entity.add_component<window>();

                if (request.hints)
                {
                    for (auto& [hint, value] : *request.hints)
                        ContextHelper::windowHint(hint, value);

                    delete request.hints;
                }
                else
                {
                    ContextHelper::windowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
                    ContextHelper::windowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
                    ContextHelper::windowHint(GLFW_OPENGL_FORWARD_COMPAT, GLFW_TRUE);
                    ContextHelper::windowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
                }

                if (request.size == math::ivec2(0, 0))
                    request.size = { 400, 400 };

                if (!request.name)
                    request.name = "<Args> Engine";

                window win = ContextHelper::createWindow(request.size, request.name, request.monitor, request.share);
                if (!win)
                {
                    const char* description;
                    int code = ContextHelper::getError(&description);

                    if (description)
                        std::cout << "GLFW ERROR " << code << ": " << description << std::endl;
                }
                ContextHelper::setWindowCloseCallback(win, &WindowSystem::closeWindow);

                handle.write(win, std::memory_order_relaxed);
                m_windowComponents.insert(win, handle);
            }
        }

        void updateWindows(time::time_span<fast_time> deltaTime)
        {
            createWindows();

            static auto query = createQuery<window>();

            for (auto entity : query)
            {
                window win = entity.get_component<window>().read();
                ContextHelper::swapBuffers(win);
            }

            ContextHelper::pollEvents();

            if (!m_ecs->world.has_component<window>())
            {
                m_eventBus->raiseEvent<events::exit>();
            }
        }
    };
}
