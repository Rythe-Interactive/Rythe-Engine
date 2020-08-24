#pragma once
#include <application/window/window.hpp>
#include <application/context/contexthelper.hpp>
#include <application/events/windowevents.hpp>

/**@file windowsystem.hpp
*/

namespace args::application
{
    class WindowSystem final : public System<WindowSystem>
    {
    private:
        inline static sparse_map<GLFWwindow*, ecs::component_handle<window>> m_windowComponents;
        inline static sparse_map<GLFWwindow*, events::EventBus*> m_windowEventBus;
        inline static async::readonly_rw_spinlock creationLock;

        static void closeWindow(GLFWwindow* win)
        {
            async::readwrite_guard guard(creationLock);

            auto handle = m_windowComponents[win];

            if (handle.valid())
            {
                m_windowEventBus[win]->raiseEvent<window_close>(m_windowComponents[win]);

                if (!ContextHelper::windowShouldClose(win))
                    return;

                handle.destroy();
                m_windowComponents.erase(win);
            }

            ContextHelper::destroyWindow(win);
        }

        static void onWindowMoved(GLFWwindow* win, int x, int y)
        {
            m_windowEventBus[win]->raiseEvent<window_move>(m_windowComponents[win], math::ivec2(x, y));
        }

        static void onWindowResize(GLFWwindow* win, int width, int height)
        {
            m_windowEventBus[win]->raiseEvent<window_resize>(m_windowComponents[win], math::ivec2(width, height));
        }

        static void onWindowRefresh(GLFWwindow* win)
        {
            m_windowEventBus[win]->raiseEvent<window_refresh>(m_windowComponents[win]);
        }

        static void onWindowFocus(GLFWwindow* win, int focused)
        {
            m_windowEventBus[win]->raiseEvent<window_focus>(m_windowComponents[win], focused);
        }

        static void onWindowIconify(GLFWwindow* win, int iconified)
        {
            m_windowEventBus[win]->raiseEvent<window_iconified>(m_windowComponents[win], iconified);
        }

        static void onWindowMaximize(GLFWwindow* win, int maximized)
        {
            m_windowEventBus[win]->raiseEvent<window_maximized>(m_windowComponents[win], maximized);
        }

        static void onWindowFrameBufferResize(GLFWwindow* win, int width, int height)
        {
            m_windowEventBus[win]->raiseEvent<window_framebuffer_resize>(m_windowComponents[win], math::ivec2(width, height));
        }

        static void onWindowContentRescale(GLFWwindow* win, float xscale, float yscale)
        {
            m_windowEventBus[win]->raiseEvent<window_content_rescale>(m_windowComponents[win], math::vec2(xscale, xscale));
        }

    public:
        virtual void setup()
        {
            auto handle = m_ecs->world.add_component<window_request>();
            handle.write({ {0,0}, nullptr, nullptr, nullptr, 1, true, nullptr }, std::memory_order_relaxed);

            createProcess<&WindowSystem::refreshWindows>("Rendering");
            createProcess<&WindowSystem::handleWindowEvents>("Input");

            m_eventBus->bindToEvent<events::exit>([](events::exit* event)
                {
                    ContextHelper::terminate();
                });
        }

        void createWindows()
        {
            static auto query = createQuery<window_request>();
            if (query.size())
                std::cout << query.size() << " window requests" << std::endl;

            for (auto entity : query)
            {
                window_request request = entity.get_component<window_request>().read();
                entity.remove_component<window_request>();

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

                ecs::component_handle<window> handle;

                {
                    async::readwrite_guard guard(creationLock);
                    std::cout << "creating a window" << std::endl;
                    handle = entity.add_component<window>();
                    handle.write(win, std::memory_order_relaxed);
                }

                m_windowComponents.insert(win, handle);

                window context = ContextHelper::getCurrentContext();

                ContextHelper::makeContextCurrent(win);
                ContextHelper::swapInterval(request.swapInterval);
                if (!request.makeCurrent)
                    ContextHelper::makeContextCurrent(context);//should be moved to rendering thread.

                ContextHelper::setWindowCloseCallback(win, &WindowSystem::closeWindow);
                ContextHelper::setWindowPosCallback(win, &WindowSystem::onWindowMoved);
                ContextHelper::setWindowSizeCallback(win, &WindowSystem::onWindowResize);
                ContextHelper::setWindowRefreshCallback(win, &WindowSystem::onWindowRefresh);
                ContextHelper::setWindowFocusCallback(win, &WindowSystem::onWindowFocus);
                ContextHelper::setWindowIconifyCallback(win, &WindowSystem::onWindowIconify);
                ContextHelper::setWindowMaximizeCallback(win, &WindowSystem::onWindowMaximize);
                ContextHelper::setFramebufferSizeCallback(win, &WindowSystem::onWindowFrameBufferResize);
                ContextHelper::setWindowContentScaleCallback(win, &WindowSystem::onWindowContentRescale);
            }
        }

        void refreshWindows(time::time_span<fast_time> deltaTime)
        {
            if (!ContextHelper::initialized())
                return;

            static auto query = createQuery<window>();

            async::readonly_guard guard(creationLock);

            for (auto entity : query)
            {
                window win = entity.get_component<window>().read(std::memory_order_relaxed);
                ContextHelper::swapBuffers(win);
            }
        }

        void handleWindowEvents(time::time_span<fast_time> deltaTime)
        {
            if (!ContextHelper::initialized())
                ContextHelper::init();

            createWindows();

            ContextHelper::pollEvents();

            static auto query = createQuery<window>();

            if (!m_ecs->world.has_component<window>())
            {
                {
                    async::readwrite_guard guard(creationLock);

                    for (auto entity : query)
                    {
                        closeWindow(entity.get_component<window>().read(std::memory_order_relaxed));
                    }
                }

                m_eventBus->raiseEvent<events::exit>();
            }
        }
    };
}
