#pragma once
#include <application/context/contexthelper.hpp>

#include <vector>
#include <utility>

/**@file window.hpp
 */

namespace legion::application
{
    struct window
    {
        friend class WindowSystem;
        window(GLFWwindow* ptr) : handle(ptr) {}
        window() = default;

        GLFWwindow* handle;
        async::readonly_rw_spinlock* lock;

        operator GLFWwindow* () const { return handle; }
        window& operator=(GLFWwindow* ptr) { handle = ptr; return *this; }

        inline void enableCursor(bool enabled) const
        {
            async::readwrite_guard guard(*lock);
            ContextHelper::makeContextCurrent(handle);
            ContextHelper::setInputMode(handle, GLFW_CURSOR, enabled ? GLFW_CURSOR_NORMAL : GLFW_CURSOR_DISABLED);
            ContextHelper::makeContextCurrent(nullptr);
        }

        inline void setSwapInterval(uint interval)
        {
            async::readwrite_guard guard(*lock);
            ContextHelper::makeContextCurrent(handle);
            ContextHelper::swapInterval(interval);
            m_swapInterval = interval;
            ContextHelper::makeContextCurrent(nullptr);
        }

        inline void show() const
        {
            ContextHelper::showWindow(handle);
        }

        inline int swapInterval() const
        {
            return m_swapInterval;
        }

        inline bool isFullscreen() const
        {
            return m_isFullscreen;
        }

        inline math::ivec2 size() const
        {
            return m_size;
        }

        inline math::ivec2 framebufferSize() const
        {
            return ContextHelper::getFramebufferSize(handle);
        }

        inline const std::string& title() const
        {
            return m_title;
        }

    private:
        std::string m_title;
        bool m_isFullscreen;
        int m_swapInterval;
        math::ivec2 m_size;
    };

    const window invalid_window = {};

    struct context_guard
    {
        context_guard(window win) : m_win(win)
        {
            win.lock->lock(async::write);
            ContextHelper::makeContextCurrent(win);
        }

        context_guard() = delete;
        context_guard(const context_guard&) = delete;
        context_guard(context_guard&&) = delete;

        ~context_guard()
        {
            ContextHelper::makeContextCurrent(nullptr);
            m_win.lock->unlock(async::write);
        }

    private:
        window m_win;
    };

}


#if !defined(DOXY_EXCLUDE)
namespace std
{
    template<>
    struct hash<legion::application::window>
    {
        std::size_t operator()(legion::application::window const& win) const noexcept
        {
            std::size_t hash;
            std::size_t h1 = std::hash<intptr_t>{}(reinterpret_cast<intptr_t>(win.handle));
            std::size_t h2 = std::hash<intptr_t>{}(reinterpret_cast<intptr_t>(win.lock));
            return h1 ^ (h2 << 1);
        }
    };
}
#endif
