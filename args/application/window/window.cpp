#include <application/window/window.hpp>
#include <iostream>

namespace args::application
{
    void window::create(int width, int height, const char* title, GLFWmonitor* monitor, GLFWwindow* share)
    {
        if (!(handle = glfwCreateWindow(width, height, title, monitor, share)))
            std::cout << "Could not create window" << std::endl;
    }

    void window::close()
    {
        glfwSetWindowShouldClose(handle, GLFW_TRUE);
        glfwDestroyWindow(handle);
        handle = nullptr;
    }

    math::ivec2 window::getFramebufferSize()
    {
        int width, height;
        glfwGetFramebufferSize(handle, &width, &height);
        return math::ivec2(width, height);
    }

    float window::getAspectRatio()
    {
        math::ivec2 size = getFramebufferSize();
        return size.x / (float)size.y;
    }

    void window::display()
    {
        glfwSwapBuffers(handle);
    }

    void window::setSwapInterval(int interval)
    {
        glfwSwapInterval(interval);
    }

    void window::makeCurrent()
    {
        glfwMakeContextCurrent(handle);
    }

    void window::setKeyCallback(GLFWkeyfun callback)
    {
        glfwSetKeyCallback(handle, callback);
    }

    void window::setWindowCloseCallback(GLFWwindowclosefun callback)
    {
        glfwSetWindowCloseCallback(handle, callback);
    }

    void window::setJoystickCallback(GLFWjoystickfun callback)
    {
        glfwSetJoystickCallback(callback);
    }

}
