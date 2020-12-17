#pragma once
#include <core/core.hpp>

#if !defined(DOXY_EXCLUDE)
#include <application/context/detail/glad/glad.h>
#include <glfw/glfw3.h>
#endif

/**
 * @file contexthelper.hpp
 */

namespace legion::application
{
    using gl_id = GLuint;
    using gl_location = GLint;

    /**@class ContextHelper
     * @brief Static helper class for calling functions on the GLFW context.
     * @ref https://www.glfw.org/docs/latest/pages.html
     */
    class ContextHelper
    {
    private:
        static std::atomic_bool m_initialized;
        static async::rw_spinlock m_initCallbackLock;
        static multicast_delegate<void()> m_onInit;

        static atomic_sparse_map<GLFWwindow*, bool> m_windowInitialized;

        static std::atomic<GLFWwindow*> newFocus;

    public:
        ContextHelper() = delete;
        ~ContextHelper() = delete;

        /**@brief Checks if the context has been initialized.
         */
        static bool initialized();
        static bool init();

        /**@brief Binds callback to be executed right after the context has been initialized.
         *        The callback will be invoked immediately if the context is already initialized.
         */
        static bool addOnInitCallback(delegate<void()> callback);
        static void terminate();
        static int getError(cstring* desc);
        static GLFWmonitor* getPrimaryMonitor();
        static GLFWmonitor* getCurrentMonitor(GLFWwindow* window);
        static void setWindowMonitor(GLFWwindow* window, GLFWmonitor* monitor, math::ivec2 pos, math::ivec2 size, int refreshRate);
        static const GLFWvidmode* getPrimaryVideoMode();
        static const GLFWvidmode* getVideoMode(GLFWmonitor* monitor);
        static void windowHint(int hint, int value);
        static GLFWwindow* createWindow(math::ivec2 dim, const char* title, GLFWmonitor* monitor = nullptr, GLFWwindow* share = nullptr);
        static GLFWwindow* createWindow(int width, int height, const char* title, GLFWmonitor* monitor = nullptr, GLFWwindow* share = nullptr);
        /**@brief Request the input thread to give focus to this window.
         */
        static void showWindow(GLFWwindow* window);
        /**@brief Gives focus to the window that has requested it. (may only be called from the input thread)
         */
        static void updateWindowFocus();
        static GLFWglproc getProcAddress(cstring procname);
        static void setWindowShouldClose(GLFWwindow* window, int value);
        static int windowShouldClose(GLFWwindow* window);
        static void setWindowIcon(GLFWwindow* window, int count, const GLFWimage* images);
        static void setWindowAttrib(GLFWwindow* window, int attrib, int value);
        static void setWindowPos(GLFWwindow* window, int x, int y);
        static void setWindowPos(GLFWwindow* window, math::ivec2 pos);
        static math::ivec2 getWindowPos(GLFWwindow* window);
        static void destroyWindow(GLFWwindow* window);
        static math::ivec2 getFramebufferSize(GLFWwindow* window);
        static void swapBuffers(GLFWwindow* window);
        static void swapInterval(int interval);
        static void pollEvents();
        static bool makeContextCurrent(GLFWwindow* window);
        static GLFWwindow* getCurrentContext();
        static GLFWkeyfun setKeyCallback(GLFWwindow* window, GLFWkeyfun callback);
        static GLFWcharfun setCharCallback(GLFWwindow* window, GLFWcharfun callback);
        static GLFWcursorposfun setCursorPosCallback(GLFWwindow* window, GLFWcursorposfun callback);
        static GLFWmousebuttonfun setMouseButtonCallback(GLFWwindow* window, GLFWmousebuttonfun callback);
        static GLFWscrollfun setScrollCallback(GLFWwindow* window, GLFWscrollfun callback);
        static GLFWdropfun setDropCallback(GLFWwindow* window, GLFWdropfun callback);
        static GLFWcursorenterfun setCursorEnterCallback(GLFWwindow* window, GLFWcursorenterfun callback);
        static GLFWwindowclosefun setWindowCloseCallback(GLFWwindow* window, GLFWwindowclosefun callback);
        static GLFWwindowposfun setWindowPosCallback(GLFWwindow* window, GLFWwindowposfun callback);
        static GLFWwindowsizefun setWindowSizeCallback(GLFWwindow* window, GLFWwindowsizefun callback);
        static GLFWwindowrefreshfun setWindowRefreshCallback(GLFWwindow* window, GLFWwindowrefreshfun callback);
        static GLFWwindowfocusfun setWindowFocusCallback(GLFWwindow* window, GLFWwindowfocusfun callback);
        static GLFWwindowiconifyfun setWindowIconifyCallback(GLFWwindow* window, GLFWwindowiconifyfun callback);
        static GLFWwindowmaximizefun setWindowMaximizeCallback(GLFWwindow* window, GLFWwindowmaximizefun callback);
        static GLFWframebuffersizefun setFramebufferSizeCallback(GLFWwindow* window, GLFWframebuffersizefun callback);
        static GLFWwindowcontentscalefun setWindowContentScaleCallback(GLFWwindow* window, GLFWwindowcontentscalefun callback);
        static GLFWjoystickfun setJoystickCallback(GLFWjoystickfun callback);
        static int getGamepadSate(int jid, GLFWgamepadstate* state);
        static void updateGamepadMappings(const char* name);
        static bool joystickPresent(int jid);
        static void setInputMode(GLFWwindow* window, int mode, int value);
    };
}
