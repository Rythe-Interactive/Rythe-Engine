#pragma once
#include <application/window/window.hpp>
#include <application/context/contexthelper.hpp>
#include <application/events/windowevents.hpp>
#include "application/events/windowinputevents.hpp"

/**@file windowsystem.hpp
*/

namespace legion::application
{
    static constexpr image_import_settings default_icon_settings = { channel_format::eight_bit, image_components::rgba, false };

    /**@class WindowSystem
     * @brief The system that's responsible for raising and polling all window events,
     *        swapping the buffers of the windows and creation and destruction of windows.
     */
    class WindowSystem final : public System<WindowSystem>
    {
    private:
        static std::unordered_map<GLFWwindow*, ecs::component<window>> m_windowComponents;

        // Internal function for closing a window safely.
        static void closeWindow(GLFWwindow* window);

        static const image_handle m_defaultIcon;

#pragma region Callbacks
        static void onWindowMoved(GLFWwindow* window, int x, int y);

        static void onWindowResize(GLFWwindow* window, int width, int height);

        static void onWindowRefresh(GLFWwindow* window);

        static void onWindowFocus(GLFWwindow* window, int focused);

        static void onWindowIconify(GLFWwindow* window, int iconified);

        static void onWindowMaximize(GLFWwindow* window, int maximized);

        static void onWindowFrameBufferResize(GLFWwindow* window, int width, int height);

        static void onWindowContentRescale(GLFWwindow* window, float xscale, float yscale);

        static void onItemDroppedInWindow(GLFWwindow* window, int count, const char** paths);

        static void onMouseEnterWindow(GLFWwindow* window, int entered);

        static void onKeyInput(GLFWwindow* window, int key, int scancode, int action, int mods);

        static void onCharInput(GLFWwindow* window, uint codepoint);

        static void onMouseMoved(GLFWwindow* window, double xpos, double ypos);

        static void onMouseButton(GLFWwindow* window, int button, int action, int mods);

        static void onMouseScroll(GLFWwindow* window, double xoffset, double yoffset);

        void onExit(events::exit& event);
#pragma endregion

    public:
        //static void requestIconChange(id_type entityId, image_handle icon);
        //static void requestIconChange(id_type entityId, const std::string& iconName);

        //static void requestFullscreenToggle(id_type entityId, math::ivec2 position = { 0, 0 }, math::ivec2 size = { -1, -1 });

        static void createWindow(id_type entityId, math::ivec2 size, const std::string& name, image_handle icon, GLFWmonitor* monitor, GLFWwindow* share, int swapInterval, const std::vector<std::pair<int, int>>& hints);
        static void createWindow(id_type entityId, math::ivec2 size = { -1, -1 }, const std::string& name = "Legion Application", image_handle icon = invalid_image_handle, GLFWmonitor* monitor = nullptr, GLFWwindow* share = nullptr, int swapInterval = 0);
        static void createWindow(id_type entityId, math::ivec2 size, const std::string& name, const std::string& iconName, GLFWmonitor* monitor, GLFWwindow* share, int swapInterval, const std::vector<std::pair<int, int>>& hints);
        static void createWindow(id_type entityId, math::ivec2 size, const std::string& name, const std::string& iconName, GLFWmonitor* monitor = nullptr, GLFWwindow* share = nullptr, int swapInterval = 0);

        void setup();

        void refreshWindows(time::time_span<fast_time> deltaTime);

        void handleWindowEvents(time::time_span<fast_time> deltaTime);
    };
}
