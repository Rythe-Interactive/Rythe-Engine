#pragma once
#define ARGS_IMPORT
#include <core/core.hpp>
#include <core/platform/args_library.hpp>

#include <glfw/glfw3.h>

namespace args::application
{
    class ARGS_API ContextHelper
    {
    private:
        static std::atomic_bool m_initialized;

    public:
        ContextHelper() = delete;
        ~ContextHelper() = delete;

        static bool initialized();
        static bool init();
        static int getError(cstring* desc);
        static GLFWmonitor* getPrimaryMonitor();
        static const GLFWvidmode* getPrimaryVideoMode();
        static const GLFWvidmode* getVideoMode(GLFWmonitor* monitor);
        static void windowHint(int hint, int value);
        static GLFWwindow* createWindow(math::ivec2 dim, const char* title, GLFWmonitor* monitor = nullptr, GLFWwindow* share = nullptr);
        static GLFWwindow* createWindow(int width, int height, const char* title, GLFWmonitor* monitor = nullptr, GLFWwindow* share = nullptr);
        static void setWindowShouldClose(GLFWwindow* window, int value);
        static void setWindowAttrib(GLFWwindow* window, int attrib, int value);
        static void setWindowPos(GLFWwindow* window, int x, int y);
        static void setWindowPos(GLFWwindow* window, math::ivec2 pos);
        static math::ivec2 getWindowPos(GLFWwindow* window);
        static void destroyWindow(GLFWwindow* window);
        static math::ivec2 getFramebufferSize(GLFWwindow* window);
        static void swapBuffers(GLFWwindow* window);
        static void swapInterval(int interval);
        static void pollEvents();
        static void makeContextCurrent(GLFWwindow* window);
        static GLFWwindow* getCurrentContext();
        static GLFWkeyfun setKeyCallback(GLFWwindow* window, GLFWkeyfun callback);
        static GLFWwindowclosefun setWindowCloseCallback(GLFWwindow* window, GLFWwindowclosefun callback);
        static GLFWwindowposfun setWindowPosCallback(GLFWwindow* window, GLFWwindowposfun callback);
        static GLFWjoystickfun setJoystickCallback(GLFWjoystickfun callback);
    };
}
