#include <application/context/contexthelper.hpp>

namespace args::application
{
    std::atomic_bool ContextHelper::m_initialized;

    bool ContextHelper::initialized()
    {
        return m_initialized.load(std::memory_order_acquire);
    }
    bool ContextHelper::init()
    {
        glfwSetErrorCallback([](int code, cstring desc) { std::cout << "GLFW ERROR " << code << ": " << desc << std::endl; });
        bool success = glfwInit();
        if (success)
            m_initialized.store(true, std::memory_order_release);
        return success;
    }

    void ContextHelper::terminate()
    {
        glfwTerminate();
        m_initialized.store(false, std::memory_order_release);
    }

    int ContextHelper::getError(cstring* desc)
    {
        return glfwGetError(desc);
    }

    GLFWmonitor* ContextHelper::getPrimaryMonitor()
    {
        return glfwGetPrimaryMonitor();
    }

    const GLFWvidmode* ContextHelper::getPrimaryVideoMode()
    {
        return glfwGetVideoMode(glfwGetPrimaryMonitor());
    }

    const GLFWvidmode* ContextHelper::getVideoMode(GLFWmonitor* monitor)
    {
        return glfwGetVideoMode(monitor);
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

    void ContextHelper::setWindowAttrib(GLFWwindow* window, int attrib, int value)
    {
        glfwSetWindowAttrib(window, attrib, value);
    }

    void ContextHelper::setWindowPos(GLFWwindow* window, int x, int y)
    {
        glfwSetWindowPos(window, x, y);
    }

    void ContextHelper::setWindowPos(GLFWwindow* window, math::ivec2 pos)
    {
        glfwSetWindowPos(window, pos.x, pos.y);
    }

    math::ivec2 ContextHelper::getWindowPos(GLFWwindow* window)
    {
        math::ivec2 pos;
        glfwGetWindowPos(window, &pos.x, &pos.y);
        return pos;
    }

    void ContextHelper::destroyWindow(GLFWwindow* window)
    {
        glfwDestroyWindow(window);
    }

    math::ivec2 ContextHelper::getFramebufferSize(GLFWwindow* window)
    {
        math::ivec2 size;
        glfwGetFramebufferSize(window, &size.x, &size.y);
        return size;
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

    GLFWwindow* ContextHelper::getCurrentContext()
    {
        return glfwGetCurrentContext();
    }

    GLFWkeyfun ContextHelper::setKeyCallback(GLFWwindow* window, GLFWkeyfun callback)
    {
        return glfwSetKeyCallback(window, callback);
    }

    GLFWwindowclosefun ContextHelper::setWindowCloseCallback(GLFWwindow* window, GLFWwindowclosefun callback)
    {
        return glfwSetWindowCloseCallback(window, callback);
    }

    GLFWwindowposfun ContextHelper::setWindowPosCallback(GLFWwindow* window, GLFWwindowposfun callback)
    {
        return glfwSetWindowPosCallback(window, callback);
    }

    GLFWjoystickfun ContextHelper::setJoystickCallback(GLFWjoystickfun callback)
    {
        return glfwSetJoystickCallback(callback);
    }

}
