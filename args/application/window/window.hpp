#pragma once
#include <glfw/glfw3.h>

#define ARGS_IMPORT
#include <core/core.hpp>
#include <core/platform/args_library.hpp>

/**@file window.hpp
 */

namespace args::application
{
    struct ARGS_API window
    {
        GLFWwindow* handle;

        void create(int width, int height, const char* title, GLFWmonitor* monitor = nullptr, GLFWwindow* share = nullptr);

        ~window();

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
