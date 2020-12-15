#pragma once
#include <application/context/contexthelper.hpp>

#include <vector>
#include <utility>

/**@file window.hpp
 */

namespace legion::application
{
    struct window
    {
        friend class WindowSystem;
        window(GLFWwindow* ptr) : handle(ptr) {}
        window() = default;

        GLFWwindow* handle;
        async::spinlock* lock;

        operator GLFWwindow* () const { return handle; }
        window& operator=(GLFWwindow* ptr) { handle = ptr; return *this; }

        inline void enableCursor(bool enabled) const
        {
            std::lock_guard guard(*lock);
            ContextHelper::makeContextCurrent(handle);
            ContextHelper::setInputMode(handle, GLFW_CURSOR, enabled ? GLFW_CURSOR_NORMAL : GLFW_CURSOR_DISABLED);
            ContextHelper::makeContextCurrent(nullptr);
        }

        inline void setSwapInterval(uint interval)
        {
            std::lock_guard guard(*lock);
            ContextHelper::makeContextCurrent(handle);
            ContextHelper::swapInterval(interval);
            m_swapInterval = interval;
            ContextHelper::makeContextCurrent(nullptr);
        }

        inline void show() const
        {
            ContextHelper::showWindow(handle);
        }

        inline int swapInterval() const
        {
            return m_swapInterval;
        }

        inline bool isFullscreen() const
        {
            return m_isFullscreen;
        }

        inline math::ivec2 size() const
        {
            return m_size;
        }

        inline const std::string& title() const
        {
            return m_title;
        }

    private:
        std::string m_title;
        bool m_isFullscreen;
        int m_swapInterval;
        math::ivec2 m_size;
    };

    const window invalid_window = {};
}
