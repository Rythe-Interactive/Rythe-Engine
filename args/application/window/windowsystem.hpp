#pragma once
#include <application/window/window.hpp>
#include <application/context/contexthelper.hpp>
#include <application/events/windowevents.hpp>
#include "application/events/windowinputevents.hpp"

/**@file windowsystem.hpp
*/

namespace args::application
{
    class WindowSystem final : public System<WindowSystem>
    {
    private:
        struct ARGS_API data
        {
            static sparse_map<GLFWwindow*, ecs::component_handle<window>> m_windowComponents;
            static sparse_map<GLFWwindow*, events::EventBus*> m_windowEventBus;
            static async::readonly_rw_spinlock m_creationLock;
        };

        ecs::EntityQuery m_windowQuery{};
        bool m_exit = false;

        async::readonly_rw_spinlock m_requestLock;
        std::vector<window_request> m_requests;

        template<typename event_type, typename... Args>
        static void raiseWindowEvent(GLFWwindow* window, Args&&... args)
        {
            if (data::m_windowEventBus.contains(window))
                data::m_windowEventBus[window]->raiseEvent<event_type>(args...);
        }

        static void closeWindow(GLFWwindow* window)
        {
            if (!ContextHelper::initialized())
                return;

            {
                async::readwrite_guard guard(data::m_creationLock);

                auto handle = data::m_windowComponents[window];

                if (handle.valid())
                {
                    raiseWindowEvent<window_close>(window, data::m_windowComponents[window]);

                    if (!ContextHelper::windowShouldClose(window))
                        return;

                    auto* lock = handle.read().lock;
                    {
                        async::readwrite_guard guard(*lock);
                        handle.write(invalid_window);
                    }
                    delete lock;

                    id_type ownerId = handle.entity;

                    handle.destroy();
                    data::m_windowComponents.erase(window);

                    if (ownerId == world_entity_id)
                    {
                        raiseWindowEvent<events::exit>(window);
                    }

                    data::m_windowEventBus.erase(window);
                }
            }

            ContextHelper::destroyWindow(window);
        }

#pragma region Callbacks
        static void onWindowMoved(GLFWwindow* window, int x, int y)
        {
            if (data::m_windowComponents.contains(window))
                raiseWindowEvent<window_move>(window, data::m_windowComponents[window], math::ivec2(x, y));
        }

        static void onWindowResize(GLFWwindow* window, int width, int height)
        {
            if (data::m_windowComponents.contains(window))
                raiseWindowEvent<window_resize>(window, data::m_windowComponents[window], math::ivec2(width, height));
        }

        static void onWindowRefresh(GLFWwindow* window)
        {
            if (data::m_windowComponents.contains(window))
                raiseWindowEvent<window_refresh>(window, data::m_windowComponents[window]);
        }

        static void onWindowFocus(GLFWwindow* window, int focused)
        {
            if (data::m_windowComponents.contains(window))
                raiseWindowEvent<window_focus>(window, data::m_windowComponents[window], focused);
        }

        static void onWindowIconify(GLFWwindow* window, int iconified)
        {
            if (data::m_windowComponents.contains(window))
                raiseWindowEvent<window_iconified>(window, data::m_windowComponents[window], iconified);
        }

        static void onWindowMaximize(GLFWwindow* window, int maximized)
        {
            if (data::m_windowComponents.contains(window))
                raiseWindowEvent<window_maximized>(window, data::m_windowComponents[window], maximized);
        }

        static void onWindowFrameBufferResize(GLFWwindow* window, int width, int height)
        {
            if (data::m_windowComponents.contains(window))
                raiseWindowEvent<window_framebuffer_resize>(window, data::m_windowComponents[window], math::ivec2(width, height));
        }

        static void onWindowContentRescale(GLFWwindow* window, float xscale, float yscale)
        {
            if (data::m_windowComponents.contains(window))
                raiseWindowEvent<window_content_rescale>(window, data::m_windowComponents[window], math::fvec2(xscale, xscale));
        }

        static void onItemDroppedInWindow(GLFWwindow* window, int count, const char** paths)
        {
            if (data::m_windowComponents.contains(window))
                raiseWindowEvent<window_item_dropped>(window, data::m_windowComponents[window], count, paths);
        }

        static void onMouseEnterWindow(GLFWwindow* window, int entered)
        {
            if (data::m_windowComponents.contains(window))
                raiseWindowEvent<mouse_enter_window>(window, data::m_windowComponents[window], entered);
        }

        static void onKeyInput(GLFWwindow* window, int key, int scancode, int action, int mods)
        {
            if (data::m_windowComponents.contains(window))
                raiseWindowEvent<key_input>(window, data::m_windowComponents[window], key, scancode, action, mods);
        }

        static void onCharInput(GLFWwindow* window, uint codepoint)
        {
            if (data::m_windowComponents.contains(window))
                raiseWindowEvent<char_input>(window, data::m_windowComponents[window], codepoint);
        }

        static void onMouseMoved(GLFWwindow* window, double xpos, double ypos)
        {
            if (data::m_windowComponents.contains(window))
                raiseWindowEvent<mouse_moved>(window, data::m_windowComponents[window], math::dvec2(xpos, ypos) / (math::dvec2)ContextHelper::getFramebufferSize(window));
        }

        static void onMouseButton(GLFWwindow* window, int button, int action, int mods)
        {
            if (data::m_windowComponents.contains(window))
                raiseWindowEvent<mouse_button>(window, data::m_windowComponents[window], button, action, mods);
        }

        static void onMouseScroll(GLFWwindow* window, double xoffset, double yoffset)
        {
            if (data::m_windowComponents.contains(window))
                raiseWindowEvent<mouse_scrolled>(window, data::m_windowComponents[window], math::dvec2(xoffset, yoffset));
        }

        void onExit(events::exit* event)
        {
            async::readwrite_guard guard(data::m_creationLock);
            for (auto entity : m_windowQuery)
            {
                ContextHelper::setWindowShouldClose(entity.get_component_handle<window>().read(std::memory_order_relaxed), true);
            }

            m_exit = true;
            ContextHelper::terminate();
        }

        void onWindowRequest(window_request* windowRequest)
        {
            if (windowRequest->entityId)
            {
                async::readwrite_guard guard(m_requestLock);
                m_requests.push_back(*windowRequest);
            }
        }
#pragma endregion

    public:
        virtual void setup()
        {
            m_windowQuery = createQuery<window>();
            bindToEvent<events::exit, &WindowSystem::onExit>();
            bindToEvent<window_request, &WindowSystem::onWindowRequest>();

            raiseEvent<window_request>(world_entity_id, math::ivec2(1360, 768), "<Args> Engine", nullptr, nullptr, 0);

            m_scheduler->sendCommand(m_scheduler->getChainThreadId("Input"), [](void* param)
                {
                    WindowSystem* self = reinterpret_cast<WindowSystem*>(param);
                    log::debug("Creating main window.");
                    self->createWindows();
                }, this);

            createProcess<&WindowSystem::refreshWindows>("Rendering");
            createProcess<&WindowSystem::handleWindowEvents>("Input");
        }

        void createWindows()
        {
            if (!ContextHelper::initialized() && !m_exit)
                if (!ContextHelper::init())
                    return;

            async::readwrite_guard guard(m_requestLock);
            for (auto& request : m_requests)
            {
                log::debug("creating a window");

                if (request.hints.size())
                {
                    for (auto& [hint, value] : request.hints)
                        ContextHelper::windowHint(hint, value);
                }
                else
                {
                    ContextHelper::windowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
                    ContextHelper::windowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
                    ContextHelper::windowHint(GLFW_OPENGL_FORWARD_COMPAT, GLFW_FALSE);
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

                ContextHelper::windowHint(GLFW_SAMPLES, 16);

                if (request.size == math::ivec2(0, 0))
                    request.size = { 400, 400 };

                if (!request.name)
                    request.name = "<Args> Engine";

                window win = ContextHelper::createWindow(request.size, request.name, request.monitor, request.share);
                win.lock = new async::readonly_rw_spinlock();

                ecs::component_handle<window> handle;

                {
                    async::readwrite_guard guard(data::m_creationLock);
                    handle = m_ecs->createComponent<window>(request.entityId, win);
                }
                log::debug("created window: {}", request.name);

                data::m_windowComponents.insert(win, handle);
                data::m_windowEventBus.insert(win, m_eventBus);

                {
                    async::readwrite_guard guard(*win.lock);
                    ContextHelper::makeContextCurrent(win);
                    ContextHelper::swapInterval(request.swapInterval);
                    ContextHelper::setWindowCloseCallback(win, &WindowSystem::closeWindow);
                    ContextHelper::setWindowPosCallback(win, &WindowSystem::onWindowMoved);
                    ContextHelper::setWindowSizeCallback(win, &WindowSystem::onWindowResize);
                    ContextHelper::setWindowRefreshCallback(win, &WindowSystem::onWindowRefresh);
                    ContextHelper::setWindowFocusCallback(win, &WindowSystem::onWindowFocus);
                    ContextHelper::setWindowIconifyCallback(win, &WindowSystem::onWindowIconify);
                    ContextHelper::setWindowMaximizeCallback(win, &WindowSystem::onWindowMaximize);
                    ContextHelper::setFramebufferSizeCallback(win, &WindowSystem::onWindowFrameBufferResize);
                    ContextHelper::setWindowContentScaleCallback(win, &WindowSystem::onWindowContentRescale);
                    ContextHelper::setDropCallback(win, &WindowSystem::onItemDroppedInWindow);
                    ContextHelper::setCursorEnterCallback(win, &WindowSystem::onMouseEnterWindow);
                    ContextHelper::setKeyCallback(win, &WindowSystem::onKeyInput);
                    ContextHelper::setCharCallback(win, &WindowSystem::onCharInput);
                    ContextHelper::setCursorPosCallback(win, &WindowSystem::onMouseMoved);
                    ContextHelper::setMouseButtonCallback(win, &WindowSystem::onMouseButton);
                    ContextHelper::setScrollCallback(win, &WindowSystem::onMouseScroll);
                    ContextHelper::makeContextCurrent(nullptr);
                }
            }

            m_requests.clear();
        }

        void refreshWindows(time::time_span<fast_time> deltaTime)
        {
            if (!ContextHelper::initialized())
                return;

            async::readonly_guard guard(data::m_creationLock);

            for (auto entity : m_windowQuery)
            {
                window win = entity.get_component_handle<window>().read(std::memory_order_relaxed);
                {
                    async::readwrite_guard guard(*win.lock);
                    ContextHelper::makeContextCurrent(win);
                    ContextHelper::swapBuffers(win);
                    ContextHelper::makeContextCurrent(nullptr);
                }
            }
        }

        void handleWindowEvents(time::time_span<fast_time> deltaTime)
        {
            createWindows();

            if (!ContextHelper::initialized())
                return;

            ContextHelper::pollEvents();
        }
    };
}
