#include <application/window/windowsystem.hpp>
#include <rendering/debugrendering.hpp>

namespace legion::application
{
    std::unordered_map<GLFWwindow*, ecs::component<window>> WindowSystem::m_windowComponents;
    const image_handle WindowSystem::m_defaultIcon = ImageCache::create_image("Legion Icon", fs::view("engine://resources/legion/icon"), { default_icon_settings });

    void WindowSystem::closeWindow(GLFWwindow* window)
    {
        if (!ContextHelper::initialized())
            return;

    }

    void WindowSystem::onWindowMoved(GLFWwindow* window, int x, int y)
    {
        if (m_windowComponents.count(window))
            events::EventBus::raiseEvent<window_move>(m_windowComponents[window], math::ivec2(x, y));
    }

    void WindowSystem::onWindowResize(GLFWwindow* win, int width, int height)
    {
        if (m_windowComponents.count(win))
        {
        }
    }

    void WindowSystem::onWindowRefresh(GLFWwindow* window)
    {
        if (m_windowComponents.count(window))
            events::EventBus::raiseEvent<window_refresh>(m_windowComponents[window]);
    }

    void WindowSystem::onWindowFocus(GLFWwindow* window, int focused)
    {
        if (m_windowComponents.count(window))
            events::EventBus::raiseEvent<window_focus>(m_windowComponents[window], focused);
    }

    void WindowSystem::onWindowIconify(GLFWwindow* window, int iconified)
    {
        if (m_windowComponents.count(window))
            events::EventBus::raiseEvent<window_iconified>(m_windowComponents[window], iconified);
    }

    void WindowSystem::onWindowMaximize(GLFWwindow* window, int maximized)
    {
        if (m_windowComponents.count(window))
            events::EventBus::raiseEvent<window_maximized>(m_windowComponents[window], maximized);
    }

    void WindowSystem::onWindowFrameBufferResize(GLFWwindow* window, int width, int height)
    {
        if (m_windowComponents.count(window))
            events::EventBus::raiseEvent<window_framebuffer_resize>(m_windowComponents[window], math::ivec2(width, height));
    }

    void WindowSystem::onWindowContentRescale(GLFWwindow* window, float xscale, float yscale)
    {
        if (m_windowComponents.count(window))
            events::EventBus::raiseEvent<window_content_rescale>(m_windowComponents[window], math::fvec2(xscale, xscale));
    }

    void WindowSystem::onItemDroppedInWindow(GLFWwindow* window, int count, const char** paths)
    {
        if (m_windowComponents.count(window))
            events::EventBus::raiseEvent<window_item_dropped>(m_windowComponents[window], count, paths);
    }

    void WindowSystem::onMouseEnterWindow(GLFWwindow* window, int entered)
    {
        if (m_windowComponents.count(window))
            events::EventBus::raiseEvent<mouse_enter_window>(m_windowComponents[window], entered);
    }

    void WindowSystem::onKeyInput(GLFWwindow* window, int key, int scancode, int action, int mods)
    {
        if (m_windowComponents.count(window))
            events::EventBus::raiseEvent<key_input>(m_windowComponents[window], key, scancode, action, mods);
    }

    void WindowSystem::onCharInput(GLFWwindow* window, uint codepoint)
    {
        if (m_windowComponents.count(window))
            events::EventBus::raiseEvent<char_input>(m_windowComponents[window], codepoint);
    }

    void WindowSystem::onMouseMoved(GLFWwindow* window, double xpos, double ypos)
    {
        if (m_windowComponents.count(window))
            events::EventBus::raiseEvent<mouse_moved>(m_windowComponents[window], math::dvec2(xpos, ypos) / (math::dvec2)ContextHelper::getFramebufferSize(window));
    }

    void WindowSystem::onMouseButton(GLFWwindow* window, int button, int action, int mods)
    {
        if (m_windowComponents.count(window))
            events::EventBus::raiseEvent<mouse_button>(m_windowComponents[window], button, action, mods);
    }

    void WindowSystem::onMouseScroll(GLFWwindow* window, double xoffset, double yoffset)
    {
        if (m_windowComponents.count(window))
            events::EventBus::raiseEvent<mouse_scrolled>(m_windowComponents[window], math::dvec2(xoffset, yoffset));
    }

    void WindowSystem::onExit(events::exit& event)
    {
        ContextHelper::terminate();
    }

    void WindowSystem::createWindow(id_type entityId, math::ivec2 size, const std::string& name, image_handle icon, GLFWmonitor* monitor, GLFWwindow* share, int swapInterval, const std::vector<std::pair<int, int>>& hints)
    {
        if (entityId)
        {
        }
        else
            log::warn("Window creation denied, invalid entity given.");
    }

    void WindowSystem::createWindow(id_type entityId, math::ivec2 size, const std::string& name, image_handle icon, GLFWmonitor* monitor, GLFWwindow* share, int swapInterval)
    {
        if (entityId)
        {
        }
        else
            log::warn("Window creation denied, invalid entity given.");
    }

    void WindowSystem::createWindow(id_type entityId, math::ivec2 size, const std::string& name, const std::string& iconName, GLFWmonitor* monitor, GLFWwindow* share, int swapInterval, const std::vector<std::pair<int, int>>& hints)
    {
        if (entityId)
        {
        }
        else
            log::warn("Window creation denied, invalid entity given.");
    }

    void WindowSystem::createWindow(id_type entityId, math::ivec2 size, const std::string& name, const std::string& iconName, GLFWmonitor* monitor, GLFWwindow* share, int swapInterval)
    {
        if (entityId)
        {
        }
        else
            log::warn("Window creation denied, invalid entity given.");
    }

    void WindowSystem::setup()
    {
        using namespace filesystem::literals;

        bindToEvent<events::exit, &WindowSystem::onExit>();

        // Create the request for the main window.

        if (!ContextHelper::initialized()) // Initialize context.
            if (!ContextHelper::init())
            {
                exit();
                return; // If we can't initialize we can't create any windows, not creating the main window means the engine should shut down.
            }
        log::trace("Creating main window.");

        createProcess<&WindowSystem::refreshWindows>("Rendering");
        createProcess<&WindowSystem::handleWindowEvents>("Input");
    }

    void WindowSystem::createWindows()
    {
        OPTICK_EVENT();
        if (m_exit) // If the engine is exiting then we can't create new windows.
            return;

        for (auto& request : m_creationRequests)
        {
            if (!m_ecs->validateEntity(request.entityId))
            {
                log::warn("Window creation denied, entity {} doesn't exist.", request.entityId);
                continue;
            }

            log::trace("creating a window");

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

            if (request.size == math::ivec2(0, 0))
                request.size = { 400, 400 };

            if (request.name.empty())
                request.name = "LEGION Engine";

            image_handle icon = request.icon;
            if (icon == invalid_image_handle)
                icon = m_defaultIcon;

            ContextHelper::windowHint(GLFW_OPENGL_DEBUG_CONTEXT, GLFW_TRUE);

            window win = ContextHelper::createWindow(request.size, request.name.c_str(), request.monitor, request.share);

            auto [lock, image] = icon.get_raw_image();
            {
                async::readonly_guard guard(lock);
                if (image.components == image_components::rgba && image.format == channel_format::eight_bit)
                {
                    GLFWimage icon{ image.size.x, image.size.y, image.get_raw_data<byte>() };

                    ContextHelper::setWindowIcon(win, 1, &icon);
                }
            }

            win.m_title = request.name;
            win.m_isFullscreen = (request.monitor != nullptr);
            win.m_swapInterval = request.swapInterval;
            win.m_size = request.size;

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

            ecs::component<window> handle(request.entityId);
            if (!m_ecs->hasComponent<window>(request.entityId))
            {
                win.lock = new async::spinlock();

                std::lock_guard wguard(*win.lock);     // This is the only code that has access to win.lock right now, so there's no deadlock risk.
                std::lock_guard cguard(m_creationLock);// Locking them both separately is faster than using a multilock.
                m_windowComponents.insert(win, handle);
                handle = m_ecs->createComponent<window>(request.entityId, win);

                log::trace("created window: {}", request.name);


                // Set all callbacks.
                setCallbacks(win);
            }
            else
            {
                handle = m_ecs->getComponent<window>(request.entityId);
                window oldWindow = handle.read();

                std::scoped_lock wguard(*oldWindow.lock, m_creationLock);

                m_windowComponents.erase(oldWindow);
                ContextHelper::destroyWindow(oldWindow);

                win.lock = oldWindow.lock;
                handle.write(win);
                m_windowComponents.insert(win, handle);

                log::trace("replaced window: {}", request.name);


                // Set all callbacks.
                setCallbacks(win);
            }
        }

        m_creationRequests.clear();
    }

    void WindowSystem::fullscreenWindows()
    {
        OPTICK_EVENT();
        if (m_exit) // If the engine is exiting then we can't change any windows.
            return;

        std::lock_guard guard(m_fullscreenRequestLock);
        for (auto& request : m_fullscreenRequests)
        {
            if (!m_ecs->validateEntity(request.entityId))
            {
                log::warn("Fullscreen toggle denied, entity {} doesn't exist.", request.entityId);
                continue;
            }

            auto handle = m_ecs->getComponent<window>(request.entityId);
            if (!handle)
            {
                log::warn("Fullscreen toggle denied, entity {} doesn't have a window.", request.entityId);
                continue;
            }

            window win = handle.read();
            std::lock_guard wguard(*win.lock);

            if (win.m_isFullscreen)
            {
                GLFWmonitor* monitor = ContextHelper::getPrimaryMonitor();
                const GLFWvidmode* mode = ContextHelper::getVideoMode(monitor);

                ContextHelper::setWindowMonitor(win, nullptr, request.position, request.size, mode->refreshRate);
                win.m_size = request.size;
                log::trace("Set window {} to windowed.", request.entityId);
            }
            else
            {
                GLFWmonitor* monitor = ContextHelper::getCurrentMonitor(win);
                const GLFWvidmode* mode = ContextHelper::getVideoMode(monitor);

                ContextHelper::setWindowMonitor(win, monitor, { 0 ,0 }, math::ivec2(mode->width, mode->height), mode->refreshRate);
                win.m_size = math::ivec2(mode->width, mode->height);
                ContextHelper::makeContextCurrent(win);
                ContextHelper::swapInterval(win.m_swapInterval);
                ContextHelper::makeContextCurrent(nullptr);
                log::trace("Set window {} to fullscreen.", request.entityId);
            }

            win.m_isFullscreen = !win.m_isFullscreen;
            handle.write(win);
        }
        m_fullscreenRequests.clear();
    }

    void WindowSystem::updateWindowIcons()
    {
        OPTICK_EVENT();
        if (m_exit) // If the engine is exiting then we can't change any windows.
            return;

        std::lock_guard guard(m_iconRequestLock);
        for (auto& request : m_iconRequests)
        {

            if (!m_ecs->validateEntity(request.entityId))
            {
                log::warn("Icon change denied, entity {} doesn't exist.", request.entityId);
                continue;
            }

            auto handle = m_ecs->getComponent<window>(request.entityId);
            if (!handle)
            {
                log::warn("Icon change denied, entity {} doesn't have a window.", request.entityId);
                continue;
            }

            auto [lock, image] = request.icon.get_raw_image();
            async::readonly_guard guard(lock);
            if (image.components != image_components::rgba || image.format != channel_format::eight_bit)
            {
                log::warn("Icon change denied, image {} has the wrong format. The needed format is 4 channels with 8 bits per channel.", request.icon.id);
                continue;
            }

            GLFWimage icon{ image.size.x, image.size.y, image.get_raw_data<byte>() };

            ContextHelper::setWindowIcon(handle.read(), 1, &icon);
        }
        m_iconRequests.clear();
    }

    void WindowSystem::refreshWindows(time::time_span<fast_time> deltaTime)
    {
        if (!ContextHelper::initialized())
            return;

        static ecs::filter<window> windowsFilter;

        for (auto entity : windowsFilter)
        {
            window& win = entity.get_component<window>();
            ContextHelper::makeContextCurrent(win);
            ContextHelper::swapBuffers(win);
            ContextHelper::makeContextCurrent(nullptr);
        }
    }

    void WindowSystem::handleWindowEvents(time::time_span<fast_time> deltaTime)
    {
        if (!ContextHelper::initialized())
            return;

        ContextHelper::pollEvents();
        ContextHelper::updateWindowFocus();
    }

}
