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
        async::readonly_rw_spinlock* lock;

        operator GLFWwindow* () const { return handle; }
        window& operator=(GLFWwindow* ptr) { handle = ptr; return *this; }

        inline void enableCursor(bool enabled)
        {
            async::readwrite_guard guard(*lock);
            ContextHelper::makeContextCurrent(handle);
            ContextHelper::setInputMode(handle, GLFW_CURSOR, enabled ? GLFW_CURSOR_NORMAL : GLFW_CURSOR_DISABLED);
            ContextHelper::makeContextCurrent(nullptr);
        }
    };

    constexpr window invalid_window = {};
}
