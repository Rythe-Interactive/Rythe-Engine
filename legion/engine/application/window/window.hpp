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
        async::spinlock* lock;

        operator GLFWwindow* () const { return handle; }
        window& operator=(GLFWwindow* ptr) { handle = ptr; return *this; }

        void enableCursor(bool enabled) const;

        void setSwapInterval(uint interval);

        void show() const;

        int swapInterval() const;

        bool isFullscreen() const;

        math::ivec2 size() const;

        math::ivec2 framebufferSize() const;

        const std::string& title() const;

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
            win.lock->lock();
            ContextHelper::makeContextCurrent(win);
        }

        context_guard() = delete;
        context_guard(const context_guard&) = delete;
        context_guard(context_guard&&) = delete;

        ~context_guard()
        {
            ContextHelper::makeContextCurrent(nullptr);
            m_win.lock->unlock();
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
