#pragma once
#include <application/window/window.hpp>
#include <application/context/contexthelper.hpp>
#include <application/events/windowevents.hpp>
#include "application/events/windowinputevents.hpp"

/**@file windowsystem.hpp
*/

namespace args::application
{
    /**@class WindowSystem
     * @brief The system that's responsible for raising and polling all window events,
     *        swapping the buffers of the windows and creation and destruction of windows.
     */
    class WindowSystem final : public System<WindowSystem>
    {
    private:
        struct ARGS_API data // Static data that needs to be exported.
        {
            static sparse_map<GLFWwindow*, ecs::component_handle<window>> m_windowComponents;
            static sparse_map<GLFWwindow*, events::EventBus*> m_windowEventBus;
            static async::readonly_rw_spinlock m_creationLock;
        };

        ecs::EntityQuery m_windowQuery{}; // Query with all the windows to update.
        bool m_exit = false; // Keep track of whether the exit event has been raised.
                             // If any window requests happen after this boolean has been set then they will be denied.

        async::readonly_rw_spinlock m_creationRequestLock; // Lock to keep the creation request list thread-safe.
        std::vector<window_request> m_creationRequests; // List of requests since the last creation loop.

        async::readonly_rw_spinlock m_fullscreenRequestLock; // Lock to keep the fullscreen request list thread-safe.
        std::vector<window_toggle_fullscreen_request> m_fullscreenRequests; // List of requests since the last fullscreen update loop.

        // Way of raising events from a static context.
        template<typename event_type, typename... Args>
        static void raiseWindowEvent(GLFWwindow* window, Args&&... args)
        {
            if (data::m_windowEventBus.contains(window))
                data::m_windowEventBus[window]->raiseEvent<event_type>(args...);
        }

        // Internal function for closing a window safely.
        static void closeWindow(GLFWwindow* window)
        {
            if (!ContextHelper::initialized())
                return;

            {
                async::readwrite_guard guard(data::m_creationLock); // Lock all creation sensitive data.

                auto handle = data::m_windowComponents[window];

                if (handle.valid())
                {
                    raiseWindowEvent<window_close>(window, data::m_windowComponents[window]); // Trigger any callbacks that want to know about any windows closing.

                    if (!ContextHelper::windowShouldClose(window)) // If a callback cancelled the window destruction then we should cancel.
                        return;

                    auto* lock = handle.read().lock;
                    {
                        async::readwrite_guard guard(*lock); // "deleting" the window is technically writing, so we copy the pointer and use that to lock it.
                        handle.write(invalid_window); // We mark the window as deleted without deleting it yet. It can cause users to find invalid windows,                                                    
                    }                                 // but at least they won't use a destroyed component after the lock unlocks.
                    delete lock;

                    id_type ownerId = handle.entity;

                    handle.destroy();
                    data::m_windowComponents.erase(window);

                    if (ownerId == world_entity_id)
                    {
                        raiseWindowEvent<events::exit>(window); // If the current window we're closing is the main window we want to close the application.
                    }                                           // (we might want to leave this up to the user at some point.)

                    data::m_windowEventBus.erase(window);
                }
            }

            ContextHelper::destroyWindow(window); // After all traces of the window throughout the engine have been erased we actually close the window.
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
                ContextHelper::setWindowShouldClose(entity.get_component_handle<window>().read(), true);
            }

            m_exit = true;
            ContextHelper::terminate();
        }

        void onWindowRequest(window_request* windowRequest)
        {
            if (windowRequest->entityId)
            {
                async::readwrite_guard guard(m_creationRequestLock);
                m_creationRequests.push_back(*windowRequest);
            }
        }

        void onFullscreenRequest(window_toggle_fullscreen_request* fullscreenRequest)
        {
            if (fullscreenRequest->entityId)
            {
                async::readwrite_guard guard(m_fullscreenRequestLock);
                m_fullscreenRequests.push_back(*fullscreenRequest);
            }
        }
#pragma endregion

    public:
        void showMainWindow()
        {
            ContextHelper::showWindow(m_ecs->world.read_component<window>());
        }

        void exit()
        {
            raiseEvent<events::exit>();
        }

        virtual void setup()
        {
            m_windowQuery = createQuery<window>();
            bindToEvent<events::exit, &WindowSystem::onExit>();
            bindToEvent<window_request, &WindowSystem::onWindowRequest>();
            bindToEvent<window_toggle_fullscreen_request, &WindowSystem::onFullscreenRequest>();

            raiseEvent<window_request>(world_entity_id, math::ivec2(1360, 768), "<Args> Engine", nullptr, nullptr, 1); // Create the request for the main window.

            m_scheduler->sendCommand(m_scheduler->getChainThreadId("Input"), [](void* param) // We send a command to the input thread before the input process chain starts.
                {                                                                            // This way we can create the main window before the rest of the engine get initialised.
                    WindowSystem* self = reinterpret_cast<WindowSystem*>(param);

                    if (!ContextHelper::initialized()) // Initialize context.
                        if (!ContextHelper::init())
                        {
                            self->exit();
                            return; // If we can't initialize we can't create any windows, not creating the main window means the engine should shut down.
                        }
                    log::debug("Creating main window.");
                    self->createWindows();
                    self->showMainWindow();
                }, this);

            createProcess<&WindowSystem::refreshWindows>("Rendering");
            createProcess<&WindowSystem::handleWindowEvents>("Input");
        }

        void createWindows()
        {
            if (m_exit) // If the engine is exiting then we can't create new windows.
                return;

            async::readwrite_guard guard(m_creationRequestLock);
            for (auto& request : m_creationRequests)
            {
                log::debug("creating a window");

                if (request.hints.size())
                {
                    for (auto& [hint, value] : request.hints)
                        ContextHelper::windowHint(hint, value);
                }
                else // Default window hints.
                {
                    ContextHelper::windowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
                    ContextHelper::windowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
                    ContextHelper::windowHint(GLFW_OPENGL_FORWARD_COMPAT, GLFW_FALSE);
                    ContextHelper::windowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

                    GLFWmonitor* monitor = request.monitor;

                    if (!request.monitor)
                        monitor = ContextHelper::getPrimaryMonitor();

                    const GLFWvidmode* mode = ContextHelper::getVideoMode(monitor);

                    ContextHelper::windowHint(GLFW_RED_BITS, mode->redBits);
                    ContextHelper::windowHint(GLFW_GREEN_BITS, mode->greenBits);
                    ContextHelper::windowHint(GLFW_BLUE_BITS, mode->blueBits);
                    ContextHelper::windowHint(GLFW_REFRESH_RATE, mode->refreshRate);
                }

                ContextHelper::windowHint(GLFW_SAMPLES, 16);

                if (request.size == math::ivec2(0, 0))
                    request.size = { 400, 400 };

                if (request.name.empty())
                    request.name = "<Args> Engine";

                window win = ContextHelper::createWindow(request.size, request.name.c_str(), request.monitor, request.share);
                win.m_title = request.name;
                win.m_isFullscreen = (request.monitor != nullptr);
                win.m_swapInterval = request.swapInterval;

                ecs::component_handle<window> handle;

                auto setCallbacks = [](const window& win)
                {
                    ContextHelper::makeContextCurrent(win);
                    ContextHelper::swapInterval(win.m_swapInterval);
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
                };

                if (!m_ecs->getEntityData(request.entityId).components.contains(typeHash<window>()))
                {
                    win.lock = new async::readonly_rw_spinlock();

                    async::readwrite_guard wguard(*win.lock);           // This is the only code that has access to win.lock right now, so there's no deadlock risk.
                    async::readwrite_guard cguard(data::m_creationLock);// Locking them both seperately is faster than using a multilock.

                    handle = m_ecs->createComponent<window>(request.entityId, win);

                    log::debug("created window: {}", request.name);

                    data::m_windowComponents.insert(win, handle);
                    data::m_windowEventBus.insert(win, m_eventBus);

                    // Set all callbacks.
                    setCallbacks(win);
                }
                else
                {
                    handle = m_ecs->getComponent<window>(request.entityId);
                    window oldWindow = handle.read();

                    async::readwrite_multiguard wguard(*oldWindow.lock, data::m_creationLock);

                    ContextHelper::destroyWindow(oldWindow);
                    data::m_windowComponents.erase(oldWindow);
                    data::m_windowEventBus.erase(oldWindow);

                    win.lock = oldWindow.lock;
                    handle.write(win);

                    log::debug("replaced window: {}", request.name);

                    data::m_windowComponents.insert(win, handle);
                    data::m_windowEventBus.insert(win, m_eventBus);

                    // Set all callbacks.
                    setCallbacks(win);
                }
            }

            m_creationRequests.clear();
        }

        void fullscreenWindows()
        {
            if (m_exit) // If the engine is exiting then we can't change any windows.
                return;

            async::readwrite_guard guard(m_fullscreenRequestLock);
            for (auto& request : m_fullscreenRequests)
            {
                auto handle = m_ecs->getComponent<window>(request.entityId);
                window win = handle.read();
                async::readwrite_guard wguard(*win.lock);

                if (win.m_isFullscreen)
                {
                    GLFWmonitor* monitor = ContextHelper::getPrimaryMonitor();
                    const GLFWvidmode* mode = ContextHelper::getVideoMode(monitor);

                    ContextHelper::setWindowMonitor(win, nullptr, request.position, request.size, mode->refreshRate);
                    log::debug("set window {} to windowed", request.entityId);
                }
                else
                {
                    GLFWmonitor* monitor = ContextHelper::getCurrentMonitor(win);
                    const GLFWvidmode* mode = ContextHelper::getVideoMode(monitor);

                    ContextHelper::setWindowMonitor(win, monitor, { 0 ,0 }, math::ivec2(mode->width, mode->height), mode->refreshRate);
                    ContextHelper::makeContextCurrent(win);
                    ContextHelper::swapInterval(win.m_swapInterval);
                    ContextHelper::makeContextCurrent(nullptr);
                    log::debug("set window {} to fullscreen", request.entityId);
                }

                win.m_isFullscreen = !win.m_isFullscreen;
                handle.write(win);
            }
            m_fullscreenRequests.clear();
        }

        void refreshWindows(time::time_span<fast_time> deltaTime)
        {
            if (!ContextHelper::initialized())
                return;

            async::readonly_guard guard(data::m_creationLock);

            for (auto entity : m_windowQuery)
            {
                window win = entity.get_component_handle<window>().read();
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
            fullscreenWindows();

            if (!ContextHelper::initialized())
                return;

            ContextHelper::pollEvents();
            ContextHelper::updateWindowFocus();
        }
    };
}
