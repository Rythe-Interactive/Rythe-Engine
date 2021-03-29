#pragma once
#include <core/core.hpp>
#include <application/window/window.hpp>
#include <vector>


namespace legion::application
{
    struct window_close final : public events::event<window_close>
    {
        ecs::component<window> windowHandle;

        window_close() = default;
        window_close(ecs::component<window> windowHandle) : windowHandle(windowHandle) {}

        
    };

    struct window_move final : public events::event<window_move>
    {
        ecs::component<window> windowHandle;
        math::ivec2 position;

        window_move() = default;
        window_move(ecs::component<window> windowHandle, math::ivec2 position) : windowHandle(windowHandle), position(position) {}

        
    };

    struct window_resize final : public events::event<window_resize>
    {
        ecs::component<window> windowHandle;
        math::ivec2 size;

        window_resize() = default;
        window_resize(ecs::component<window> windowHandle, math::ivec2 size) : windowHandle(windowHandle), size(size) {}

        
    };

    struct window_refresh final : public events::event<window_refresh>
    {
        ecs::component<window> windowHandle;

        window_refresh() = default;
        window_refresh(ecs::component<window> windowHandle) : windowHandle(windowHandle) {}

        
    };

    struct window_focus final : public events::event<window_focus>
    {
        ecs::component<window> windowHandle;
        int focused;

        window_focus() = default;
        window_focus(ecs::component<window> windowHandle, int focused) : windowHandle(windowHandle), focused(focused) {}

        
    };

    struct window_iconified final : public events::event<window_iconified>
    {
        ecs::component<window> windowHandle;
        int iconified;

        window_iconified() = default;
        window_iconified(ecs::component<window> windowHandle, int iconified) : windowHandle(windowHandle), iconified(iconified) {}

        
    };

    struct window_maximized final : public events::event<window_maximized>
    {
        ecs::component<window> windowHandle;
        int maximized;

        window_maximized() = default;
        window_maximized(ecs::component<window> windowHandle, int maximized) : windowHandle(windowHandle), maximized(maximized) {}

        
    };

    struct window_framebuffer_resize final : public events::event<window_framebuffer_resize>
    {
        ecs::component<window> windowHandle;
        math::ivec2 size;

        window_framebuffer_resize() = default;
        window_framebuffer_resize(ecs::component<window> windowHandle, math::ivec2 size) : windowHandle(windowHandle), size(size) {}

        
    };

    struct window_content_rescale final : public events::event<window_content_rescale>
    {
        ecs::component<window> windowHandle;
        math::vec2 scale;

        window_content_rescale() = default;
        window_content_rescale(ecs::component<window> windowHandle, math::vec2 scale) : windowHandle(windowHandle), scale(scale) {}

        
    };

    struct window_item_dropped final : public events::event<window_item_dropped>
    {
        ecs::component<window> windowHandle;
        std::vector<cstring> paths;

        window_item_dropped() = default;
        window_item_dropped(ecs::component<window> windowHandle, int count, cstring* cpaths) : windowHandle(windowHandle)
        {
            for (int i = 0; i < count; i++)
                paths.push_back(cpaths[i]);
        }

        
    };

    struct mouse_enter_window final : public events::event<mouse_enter_window>
    {
        ecs::component<window> windowHandle;
        int entered;

        mouse_enter_window() = default;
        mouse_enter_window(ecs::component<window> windowHandle, int entered) : windowHandle(windowHandle), entered(entered) {}

        
    };
}
