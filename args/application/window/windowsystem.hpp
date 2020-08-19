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
                window comp = handle.read(std::memory_order_relaxed);

                if (comp.parent)
                    m_windowComponents[comp.parent].read().children->erase(handle);

                for (auto& childHandle : *comp.children)
                    closeWindow(childHandle.read());

                delete comp.children;
                handle.write(nullptr, std::memory_order_relaxed);
                handle.destroy();
                m_windowComponents.erase(win);
            }
        }

        static void onWindowMoved(GLFWwindow* win, int x, int y)
        {
            auto handle = m_windowComponents[win];
            for (auto& childHandle : *handle.read().children)
            {
                auto child = childHandle.read();
                ContextHelper::setWindowPos(child, x + child.offset.x, y + child.offset.y);
            }
        }

        void dockWindow(ecs::component_handle<window> child, ecs::component_handle<window> parent)
        {
            window par = parent.read();
            par.children->insert(child);
            parent.write(par);

            window chi = child.read();
            chi.parent = par;
            child.write(chi);
            ContextHelper::setWindowAttrib(chi, GLFW_FLOATING, GLFW_TRUE);
            ContextHelper::setWindowPos(chi, ContextHelper::getWindowPos(par) + chi.offset);
        }

    public:
        virtual void setup()
        {
            ContextHelper::init();

            auto handle = m_ecs->world.add_component<window_request>();
            handle.write({ {0,0}, nullptr, nullptr, nullptr, 1, true, nullptr }, std::memory_order_relaxed);
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

                    GLFWmonitor* moniter = request.monitor;

                    if (!request.monitor)
                        moniter = ContextHelper::getPrimaryMonitor();

                    const GLFWvidmode* mode = ContextHelper::getVideoMode(moniter);

                    ContextHelper::windowHint(GLFW_RED_BITS, mode->redBits);
                    ContextHelper::windowHint(GLFW_GREEN_BITS, mode->greenBits);
                    ContextHelper::windowHint(GLFW_BLUE_BITS, mode->blueBits);
                    ContextHelper::windowHint(GLFW_REFRESH_RATE, mode->refreshRate);
                }

                if (request.size == math::ivec2(0, 0))
                    request.size = { 400, 400 };

                if (!request.name)
                    request.name = "<Args> Engine";

                window win = ContextHelper::createWindow(request.size, request.name, request.monitor, request.share);
                win.children = new hashed_sparse_set<ecs::component_handle<window>>();
                win.offset = { 0, 35 };
                win.parent = nullptr;
                handle.write(win, std::memory_order_relaxed);
                m_windowComponents.insert(win, handle);

                window context = ContextHelper::getCurrentContext();
                if (context)
                {
                    dockWindow(handle, m_windowComponents[context]);
                }

                ContextHelper::makeContextCurrent(win);
                ContextHelper::swapInterval(request.swapInterval);
                if (!request.makeCurrent)
                    ContextHelper::makeContextCurrent(context);

                ContextHelper::setWindowCloseCallback(win, &WindowSystem::closeWindow);
                ContextHelper::setWindowPosCallback(win, &WindowSystem::onWindowMoved);

            }
        }

        void updateWindows(time::time_span<fast_time> deltaTime)
        {
            createWindows();

            static auto query = createQuery<window>();

            for (auto entity : query)
            {
                window win = entity.get_component<window>().read(std::memory_order_relaxed);
                ContextHelper::swapBuffers(win);
            }

            ContextHelper::pollEvents();

            if (!m_ecs->world.has_component<window>())
            {
                for (auto entity : query)
                {
                    closeWindow(entity.get_component<window>().read(std::memory_order_relaxed));
                }

                m_eventBus->raiseEvent<events::exit>();
            }
        }
    };
}
