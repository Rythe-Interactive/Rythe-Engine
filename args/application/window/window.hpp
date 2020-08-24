#pragma once
#include <application/context/contexthelper.hpp>

#include <vector>
#include <utility>

/**@file window.hpp
 */

namespace args::application
{
    struct window_request
    {
        math::ivec2 size;
        cstring name;
        GLFWmonitor* monitor;
        GLFWwindow* share;
        int swapInterval;
        bool makeCurrent;
        std::vector<std::pair<int, int>>* hints;
    };

    struct window
    {
        window(GLFWwindow* ptr) : handle(ptr) {}
        window() = default;

        GLFWwindow* handle;
        
        operator GLFWwindow* () const { return handle; }
        window& operator=(GLFWwindow* ptr) { handle = ptr; return *this; }
    };
}
