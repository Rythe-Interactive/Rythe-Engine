#include <application/context/contexthelper.hpp>

namespace args::application
{
    bool ContextHelper::init()
    {
        return glfwInit();
    }

    int ContextHelper::getError(cstring* desc)
    {
        return glfwGetError(desc);
    }

    void ContextHelper::windowHint(int hint, int value)
    {
        glfwWindowHint(hint, value);
    }

    GLFWwindow* ContextHelper::createWindow(math::ivec2 dim, const char* title, GLFWmonitor* monitor, GLFWwindow* share)
    {
        return glfwCreateWindow(dim.x, dim.y, title, monitor, share);
    }

    GLFWwindow* ContextHelper::createWindow(int width, int height, const char* title, GLFWmonitor* monitor, GLFWwindow* share)
    {
        return glfwCreateWindow(width, height, title, monitor, share);
    }

    void ContextHelper::setWindowShouldClose(GLFWwindow* window, int value)
    {
        glfwSetWindowShouldClose(window, value);
    }

    void ContextHelper::destroyWindow(GLFWwindow* window)
    {
        glfwDestroyWindow(window);
    }

    void ContextHelper::getFramebufferSize(GLFWwindow* window, int* width, int* height)
    {
        glfwGetFramebufferSize(window, width, height);
    }

    void ContextHelper::swapBuffers(GLFWwindow* window)
    {
        glfwSwapBuffers(window);
    }

    void ContextHelper::swapInterval(int interval)
    {
        glfwSwapInterval(interval);
    }

    void ContextHelper::pollEvents()
    {
        glfwPollEvents();
    }

    void ContextHelper::makeContextCurrent(GLFWwindow* window)
    {
        glfwMakeContextCurrent(window);
    }

    GLFWkeyfun ContextHelper::setKeyCallback(GLFWwindow* window, GLFWkeyfun callback)
    {
        return glfwSetKeyCallback(window, callback);
    }

    GLFWwindowclosefun ContextHelper::setWindowCloseCallback(GLFWwindow* window, GLFWwindowclosefun callback)
    {
        return glfwSetWindowCloseCallback(window, callback);
    }

    GLFWjoystickfun ContextHelper::setJoystickCallback(GLFWjoystickfun callback)
    {
        return glfwSetJoystickCallback(callback);
    }

}
