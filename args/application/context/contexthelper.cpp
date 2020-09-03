#include <application/context/contexthelper.hpp>

namespace args::application
{
    std::atomic_bool ContextHelper::m_initialized;
    atomic_sparse_map<GLFWwindow*, bool> ContextHelper::m_windowInitialized;

    async::readonly_rw_spinlock ContextHelper::m_initCallbackLock;
    multicast_delegate<void()> ContextHelper::m_onInit;

    bool ContextHelper::initialized()
    {
        return m_initialized.load(std::memory_order_acquire);
    }

    bool ContextHelper::init()
    {
        glfwSetErrorCallback([](int code, cstring desc)
            {
                std::cout << "GLFW ERROR " << code << ": " << desc << std::endl;
            });

        bool success = glfwInit();
        if (success)
        {
            m_initialized.store(true, std::memory_order_release);
            async::readonly_guard guard(m_initCallbackLock);
            m_onInit.invoke();
        }

        return success;
    }

    bool ContextHelper::addOnInitCallback(delegate<void()> callback)
    {
        if (initialized()) {
            callback();
            return false;
        }

        async::readwrite_guard guard(m_initCallbackLock);
        m_onInit += callback;
        return true;
    }

    void ContextHelper::terminate()
    {
        m_initialized.store(false, std::memory_order_release);
        glfwTerminate();
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
        GLFWwindow* window = glfwCreateWindow(dim.x, dim.y, title, monitor, share);

        auto* context = glfwGetCurrentContext();

        glfwMakeContextCurrent(window);

        if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
        {
            std::cout << "Failed to load OpenGL" << std::endl;
        }
        else
        {
            glEnable(GL_DEBUG_OUTPUT);
            glDebugMessageCallback([](GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length, const GLchar* message, const void* userParam)
                {
                    cstring s;
                    switch (source)
                    {
                    case GL_DEBUG_SOURCE_API:
                        s = "GL_DEBUG_SOURCE_API";
                        break;
                    case GL_DEBUG_SOURCE_SHADER_COMPILER:
                        s = "GL_DEBUG_SOURCE_SHADER_COMPILER";
                        break;
                    case GL_DEBUG_SOURCE_THIRD_PARTY:
                        s = "GL_DEBUG_SOURCE_THIRD_PARTY";
                        break;
                    case GL_DEBUG_SOURCE_APPLICATION:
                        s = "GL_DEBUG_SOURCE_APPLICATION";
                        break;
                    case GL_DEBUG_SOURCE_OTHER:
                        s = "GL_DEBUG_SOURCE_OTHER";
                        break;
                    default:
                        s = "UNKNOWN SOURCE";
                        break;
                    }

                    cstring t;

                    switch (type)
                    {
                    case GL_DEBUG_TYPE_ERROR:
                        t = "GL_DEBUG_TYPE_ERROR";
                        break;
                    case GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR:
                        t = "GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR";
                        break;
                    case GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR:
                        t = "GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR";
                        break;
                    case GL_DEBUG_TYPE_PERFORMANCE:
                        t = "GL_DEBUG_TYPE_PERFORMANCE";
                        break;
                    case GL_DEBUG_TYPE_PORTABILITY:
                        t = "GL_DEBUG_TYPE_PORTABILITY";
                        break;
                    case GL_DEBUG_TYPE_MARKER:
                        t = "GL_DEBUG_TYPE_MARKER";
                        break;
                    case GL_DEBUG_TYPE_PUSH_GROUP:
                        t = "GL_DEBUG_TYPE_PUSH_GROUP";
                        break;
                    case GL_DEBUG_TYPE_POP_GROUP:
                        t = "GL_DEBUG_TYPE_POP_GROUP";
                        break;
                    case GL_DEBUG_TYPE_OTHER:
                        t = "GL_DEBUG_TYPE_OTHER";
                        break;
                    default:
                        t = "UNKNOWN TYPE";
                        break;
                    }

                    cstring sev;
                    switch (severity)
                    {
                    case GL_DEBUG_SEVERITY_HIGH:
                        sev = "GL_DEBUG_SEVERITY_HIGH ";
                        break;
                    case GL_DEBUG_SEVERITY_MEDIUM:
                        sev = "GL_DEBUG_SEVERITY_MEDIUM ";
                        break;
                    case GL_DEBUG_SEVERITY_LOW:
                        sev = "GL_DEBUG_SEVERITY_LOW ";
                        break;
                    case GL_DEBUG_SEVERITY_NOTIFICATION:
                        sev = "GL_DEBUG_SEVERITY_NOTIFICATION ";
                        break;
                    default:
                        sev = "UNKNOWN SEVERITY";
                        break;
                    }


                    std::printf("GL CALLBACK: %s source = %s type = %s, severity = %s, message = %s\n", (type == GL_DEBUG_TYPE_ERROR ? " GL ERROR " : ""), s, t, sev, message);
                }, nullptr);
        }

        std::cout << "loaded OpenGL version: " << GLVersion.major << '.' << GLVersion.minor << std::endl;

        glfwMakeContextCurrent(context);

        return window;
    }

    GLFWwindow* ContextHelper::createWindow(int width, int height, const char* title, GLFWmonitor* monitor, GLFWwindow* share)
    {
        GLFWwindow* window = glfwCreateWindow(width, height, title, monitor, share);

        auto* context = glfwGetCurrentContext();

        glfwMakeContextCurrent(window);

        if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
        {
            std::cout << "Failed to load OpenGL" << std::endl;
        }

        std::cout << "loaded OpenGL version: " << GLVersion.major << '.' << GLVersion.minor << std::endl;

        glfwMakeContextCurrent(context);

        return window;
    }

    void ContextHelper::setWindowShouldClose(GLFWwindow* window, int value)
    {
        glfwSetWindowShouldClose(window, value);
    }

    int ContextHelper::windowShouldClose(GLFWwindow* window)
    {
        return glfwWindowShouldClose(window);
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
        if (initialized())
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
        if (initialized())
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

    GLFWcharfun ContextHelper::setCharCallback(GLFWwindow* window, GLFWcharfun callback)
    {
        return glfwSetCharCallback(window, callback);
    }

    GLFWcursorposfun ContextHelper::setCursorPosCallback(GLFWwindow* window, GLFWcursorposfun callback)
    {
        return glfwSetCursorPosCallback(window, callback);
    }

    GLFWmousebuttonfun ContextHelper::setMouseButtonCallback(GLFWwindow* window, GLFWmousebuttonfun callback)
    {
        return glfwSetMouseButtonCallback(window, callback);
    }

    GLFWscrollfun ContextHelper::setScrollCallback(GLFWwindow* window, GLFWscrollfun callback)
    {
        return glfwSetScrollCallback(window, callback);
    }

    GLFWdropfun ContextHelper::setDropCallback(GLFWwindow* window, GLFWdropfun callback)
    {
        return glfwSetDropCallback(window, callback);
    }

    GLFWcursorenterfun ContextHelper::setCursorEnterCallback(GLFWwindow* window, GLFWcursorenterfun callback)
    {
        return glfwSetCursorEnterCallback(window, callback);
    }

    GLFWwindowclosefun ContextHelper::setWindowCloseCallback(GLFWwindow* window, GLFWwindowclosefun callback)
    {
        return glfwSetWindowCloseCallback(window, callback);
    }

    GLFWwindowposfun ContextHelper::setWindowPosCallback(GLFWwindow* window, GLFWwindowposfun callback)
    {
        return glfwSetWindowPosCallback(window, callback);
    }

    GLFWwindowsizefun ContextHelper::setWindowSizeCallback(GLFWwindow* window, GLFWwindowsizefun callback)
    {
        return glfwSetWindowSizeCallback(window, callback);
    }

    GLFWwindowrefreshfun ContextHelper::setWindowRefreshCallback(GLFWwindow* window, GLFWwindowrefreshfun callback)
    {
        return glfwSetWindowRefreshCallback(window, callback);
    }

    GLFWwindowfocusfun ContextHelper::setWindowFocusCallback(GLFWwindow* window, GLFWwindowfocusfun callback)
    {
        return glfwSetWindowFocusCallback(window, callback);
    }

    GLFWwindowiconifyfun ContextHelper::setWindowIconifyCallback(GLFWwindow* window, GLFWwindowiconifyfun callback)
    {
        return glfwSetWindowIconifyCallback(window, callback);
    }

    GLFWwindowmaximizefun ContextHelper::setWindowMaximizeCallback(GLFWwindow* window, GLFWwindowmaximizefun callback)
    {
        return glfwSetWindowMaximizeCallback(window, callback);
    }

    GLFWframebuffersizefun ContextHelper::setFramebufferSizeCallback(GLFWwindow* window, GLFWframebuffersizefun callback)
    {
        return glfwSetFramebufferSizeCallback(window, callback);
    }

    GLFWwindowcontentscalefun ContextHelper::setWindowContentScaleCallback(GLFWwindow* window, GLFWwindowcontentscalefun callback)
    {
        return glfwSetWindowContentScaleCallback(window, callback);
    }

    GLFWjoystickfun ContextHelper::setJoystickCallback(GLFWjoystickfun callback)
    {
        return glfwSetJoystickCallback(callback);
    }


    int ContextHelper::getGamepadSate(int jid, GLFWgamepadstate* state)
    {
        return glfwGetGamepadState(jid, state);
    }

    void ContextHelper::updateGamepadMappings(const char* name)
    {
        (void)glfwUpdateGamepadMappings(name);
    }

    bool ContextHelper::joystickPresent(int jid)
    {
        return glfwJoystickPresent(jid);
    }

}
