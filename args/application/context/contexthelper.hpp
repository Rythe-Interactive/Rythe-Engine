#pragma once
#define ARGS_IMPORT
#include <core/core.hpp>
#include <core/platform/args_library.hpp>

#include <glfw/glfw3.h>

namespace args::application
{
    class ARGS_API ContextHelper
    {
    public:
        ContextHelper() = delete;
        ~ContextHelper() = delete;

        static bool init();
        static void windowHint(int hint, int value);
        static GLFWwindow* createWindow(math::ivec2 dim, const char* title, GLFWmonitor* monitor = nullptr, GLFWwindow* share = nullptr);
        static GLFWwindow* createWindow(int width, int height, const char* title, GLFWmonitor* monitor = nullptr, GLFWwindow* share = nullptr);
        static void setWindowShouldClose(GLFWwindow* window, int value);
        static void destroyWindow(GLFWwindow* window);
        static void getFramebufferSize(GLFWwindow* window, int* width, int* height);
        static void swapBuffers(GLFWwindow* window);
        static void swapInterval(int interval);
        static void makeContextCurrent(GLFWwindow* window);
        static GLFWkeyfun setKeyCallback(GLFWwindow* window, GLFWkeyfun callback);
        static GLFWwindowclosefun setWindowCloseCallback(GLFWwindow* window, GLFWwindowclosefun callback);
        static GLFWjoystickfun setJoystickCallback(GLFWjoystickfun callback);
    };
}
