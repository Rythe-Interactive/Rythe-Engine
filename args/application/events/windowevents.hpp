#pragma once
#include <core/events/event.hpp>
#include <application/window/window.hpp>

namespace args::application
{
    struct window_close final : public events::event<window_close>
    {
        ecs::component_handle<window> windowHandle;

        window_close(ecs::component_handle<window> windowHandle) : windowHandle(windowHandle) {}

        virtual bool persistent() { return false; }
        virtual bool unique() { return false; }
    };

    struct window_move final : public events::event<window_move>
    {
        ecs::component_handle<window> windowHandle;
        math::ivec2 position;

        window_move(ecs::component_handle<window> windowHandle, math::ivec2 position) : windowHandle(windowHandle), position(position) {}

        virtual bool persistent() { return false; }
        virtual bool unique() { return false; }
    };

    struct window_resize final : public events::event<window_resize>
    {
        ecs::component_handle<window> windowHandle;
        math::ivec2 size;

        window_resize(ecs::component_handle<window> windowHandle, math::ivec2 size) : windowHandle(windowHandle), size(size) {}

        virtual bool persistent() { return false; }
        virtual bool unique() { return false; }
    };

    struct window_refresh final : public events::event<window_refresh>
    {
        ecs::component_handle<window> windowHandle;

        window_refresh(ecs::component_handle<window> windowHandle) : windowHandle(windowHandle) {}

        virtual bool persistent() { return false; }
        virtual bool unique() { return false; }
    };
    
    struct window_focus final : public events::event<window_focus>
    {
        ecs::component_handle<window> windowHandle;
        int focused;

        window_focus(ecs::component_handle<window> windowHandle, int focused) : windowHandle(windowHandle), focused(focused) {}

        virtual bool persistent() { return false; }
        virtual bool unique() { return false; }
    };

    struct window_iconified final : public events::event<window_iconified>
    {
        ecs::component_handle<window> windowHandle;
        int iconified;

        window_iconified(ecs::component_handle<window> windowHandle, int iconified) : windowHandle(windowHandle), iconified(iconified) {}

        virtual bool persistent() { return false; }
        virtual bool unique() { return false; }
    };

    struct window_maximized final : public events::event<window_maximized>
    {
        ecs::component_handle<window> windowHandle;
        int maximized;

        window_maximized(ecs::component_handle<window> windowHandle, int maximized) : windowHandle(windowHandle), maximized(maximized) {}

        virtual bool persistent() { return false; }
        virtual bool unique() { return false; }
    };

    struct window_framebuffer_resize final : public events::event<window_framebuffer_resize>
    {
        ecs::component_handle<window> windowHandle;
        math::ivec2 size;

        window_framebuffer_resize(ecs::component_handle<window> windowHandle, math::ivec2 size) : windowHandle(windowHandle), size(size) {}

        virtual bool persistent() { return false; }
        virtual bool unique() { return false; }
    };

    struct window_content_rescale final : public events::event<window_content_rescale>
    {
        ecs::component_handle<window> windowHandle;
        math::vec2 scale;

        window_content_rescale(ecs::component_handle<window> windowHandle, math::vec2 scale) : windowHandle(windowHandle), scale(scale) {}

        virtual bool persistent() { return false; }
        virtual bool unique() { return false; }
    };
}
