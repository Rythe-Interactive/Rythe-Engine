#include <application/window/window.hpp>
#include <iostream>

namespace args::application
{
    void window::create(int width, int height, const char* title, GLFWmonitor* monitor, GLFWwindow* share)
    {
        if (!(handle = ContextHelper::createWindow(width, height, title, monitor, share)))
            std::cout << "Could not create window" << std::endl;
    }

    void window::close()
    {
        ContextHelper::setWindowShouldClose(handle, GLFW_TRUE);
        ContextHelper::destroyWindow(handle);
        handle = nullptr;
    }

    math::ivec2 window::getFramebufferSize()
    {
        int width, height;
        ContextHelper::getFramebufferSize(handle, &width, &height);
        return math::ivec2(width, height);
    }

    float window::getAspectRatio()
    {
        math::ivec2 size = getFramebufferSize();
        return size.x / (float)size.y;
    }

    void window::display()
    {
        ContextHelper::swapBuffers(handle);
    }

    void window::setSwapInterval(int interval)
    {
        ContextHelper::swapInterval(interval);
    }

    void window::makeCurrent()
    {
        ContextHelper::makeContextCurrent(handle);
    }

    void window::setKeyCallback(GLFWkeyfun callback)
    {
        ContextHelper::setKeyCallback(handle, callback);
    }

    void window::setWindowCloseCallback(GLFWwindowclosefun callback)
    {
        ContextHelper::setWindowCloseCallback(handle, callback);
    }

    void window::setJoystickCallback(GLFWjoystickfun callback)
    {
        ContextHelper::setJoystickCallback(callback);
    }

}
