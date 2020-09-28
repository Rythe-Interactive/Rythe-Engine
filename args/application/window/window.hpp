#pragma once
#include <application/context/contexthelper.hpp>

#include <vector>
#include <utility>

/**@file window.hpp
 */

namespace args::application
{
    struct window
    {
        window(GLFWwindow* ptr) : handle(ptr) {}
        window() = default;

        GLFWwindow* handle;
        cstring title;
        bool isFullscreen;
        async::readonly_rw_spinlock* lock;
        int swapInterval;

        operator GLFWwindow* () const { return handle; }
        window& operator=(GLFWwindow* ptr) { handle = ptr; return *this; }

        inline void enableCursor(bool enabled) const
        {
            async::readwrite_guard guard(*lock);
            ContextHelper::makeContextCurrent(handle);
            ContextHelper::setInputMode(handle, GLFW_CURSOR, enabled ? GLFW_CURSOR_NORMAL : GLFW_CURSOR_DISABLED);
            ContextHelper::makeContextCurrent(nullptr);
        }

        inline void setSwapInterval(uint interval)
        {
            async::readwrite_guard guard(*lock);
            ContextHelper::makeContextCurrent(handle);
            ContextHelper::swapInterval(interval);
            swapInterval = interval;
            ContextHelper::makeContextCurrent(nullptr);
        }

        inline void show() const
        {
            ContextHelper::showWindow(handle);
        }
    };

    constexpr window invalid_window = {};
}
