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
        GLFWwindow* nativeHandle;

        operator GLFWwindow* () const { return nativeHandle; }
        window& operator=(GLFWwindow* ptr) { nativeHandle = ptr; return *this; }

        void enableCursor(bool enabled) const;

        void setSwapInterval(uint interval);

        void show() const;

        int swapInterval() const;

        bool isFullscreen() const;

        math::ivec2 size() const;

        math::ivec2 framebufferSize() const;

        const std::string& title() const;

        std::string m_title;
        bool m_isFullscreen;
        int m_swapInterval;
        math::ivec2 m_size;
    };

    const window invalid_window = {};

    struct context_guard
    {
    private:
        bool m_contextIsValid = false;

    public:
        context_guard(window win);
        bool contextIsValid() { return m_contextIsValid; }

        context_guard() = delete;
        context_guard(const context_guard&) = delete;
        context_guard(context_guard&&) = delete;

        ~context_guard();

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
            return std::hash<intptr_t>{}(reinterpret_cast<intptr_t>(win.nativeHandle));
        }
    };
}
#endif
