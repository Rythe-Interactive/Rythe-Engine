#pragma once
#include <application/context/contexthelper.hpp>

/**@file window.hpp
 */

namespace args::application
{
    struct ARGS_API window
    {
        window() = default;
        window(const window&) = default;
        window(window&&) = default;
        ~window() = default;

        window& operator=(const window&) = default;
        window& operator=(window&&) = default;

        GLFWwindow* handle;

        void create(int width, int height, const char* title, GLFWmonitor* monitor = nullptr, GLFWwindow* share = nullptr);
        void close();

        operator GLFWwindow* () const { return handle; }

        math::ivec2 getFramebufferSize();        

        float getAspectRatio();        

        void display();

        void setSwapInterval(int interval);
        
        void makeCurrent();

        void setKeyCallback(GLFWkeyfun callback);

        void setWindowCloseCallback(GLFWwindowclosefun callback);

        void setJoystickCallback(GLFWjoystickfun callback);
    };
}
