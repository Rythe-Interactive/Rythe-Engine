#pragma once
#include <core/events/event.hpp>
#include <application/window/window.hpp>
#include <vector>


namespace args::application
{
    struct window_request final : public events::event<window_request>
    {
        id_type entityId;
        math::ivec2 size;
        std::string name;
        GLFWmonitor* monitor;
        GLFWwindow* share;
        int swapInterval;
        std::vector<std::pair<int, int>> hints;

        window_request(id_type entityId, math::ivec2 size, cstring name, GLFWmonitor* monitor, GLFWwindow* share, int swapInterval, const std::vector<std::pair<int, int>>& hints)
            : entityId(entityId), size(size), name(name), monitor(monitor), share(share), swapInterval(swapInterval), hints(hints)
        {}
        window_request(id_type entityId = invalid_id, math::ivec2 size = { 400, 400 }, cstring name = "<Args> Engine", GLFWmonitor* monitor = nullptr, GLFWwindow* share = nullptr, int swapInterval = 0)
            : entityId(entityId), size(size), name(name), monitor(monitor), share(share), swapInterval(swapInterval)
        {}

        virtual bool persistent() override { return false; }
        virtual bool unique() override { return false; }
    };

    struct window_toggle_fullscreen_request final : public events::event<window_toggle_fullscreen_request>
    {
        id_type entityId;
        math::ivec2 position;
        math::ivec2 size;

        window_toggle_fullscreen_request(id_type entityId = invalid_id, math::ivec2 position = { 100 ,100 }, math::ivec2 size = { 400, 400 }) : entityId(entityId), position(position), size(size) {}

        virtual bool persistent() override { return false; }
        virtual bool unique() override { return false; }
    };

    struct window_close final : public events::event<window_close>
    {
        ecs::component_handle<window> windowHandle;

        window_close() = default;
        window_close(ecs::component_handle<window> windowHandle) : windowHandle(windowHandle) {}

        virtual bool persistent() override { return false; }
        virtual bool unique() override { return false; }
    };

    struct window_move final : public events::event<window_move>
    {
        ecs::component_handle<window> windowHandle;
        math::ivec2 position;

        window_move() = default;
        window_move(ecs::component_handle<window> windowHandle, math::ivec2 position) : windowHandle(windowHandle), position(position) {}

        virtual bool persistent() override { return false; }
        virtual bool unique() override { return false; }
    };

    struct window_resize final : public events::event<window_resize>
    {
        ecs::component_handle<window> windowHandle;
        math::ivec2 size;

        window_resize() = default;
        window_resize(ecs::component_handle<window> windowHandle, math::ivec2 size) : windowHandle(windowHandle), size(size) {}

        virtual bool persistent() override { return false; }
        virtual bool unique() override { return false; }
    };

    struct window_refresh final : public events::event<window_refresh>
    {
        ecs::component_handle<window> windowHandle;

        window_refresh() = default;
        window_refresh(ecs::component_handle<window> windowHandle) : windowHandle(windowHandle) {}

        virtual bool persistent() override { return false; }
        virtual bool unique() override { return false; }
    };

    struct window_focus final : public events::event<window_focus>
    {
        ecs::component_handle<window> windowHandle;
        int focused;

        window_focus() = default;
        window_focus(ecs::component_handle<window> windowHandle, int focused) : windowHandle(windowHandle), focused(focused) {}

        virtual bool persistent() override { return false; }
        virtual bool unique() override { return false; }
    };

    struct window_iconified final : public events::event<window_iconified>
    {
        ecs::component_handle<window> windowHandle;
        int iconified;

        window_iconified() = default;
        window_iconified(ecs::component_handle<window> windowHandle, int iconified) : windowHandle(windowHandle), iconified(iconified) {}

        virtual bool persistent() override { return false; }
        virtual bool unique() override { return false; }
    };

    struct window_maximized final : public events::event<window_maximized>
    {
        ecs::component_handle<window> windowHandle;
        int maximized;

        window_maximized() = default;
        window_maximized(ecs::component_handle<window> windowHandle, int maximized) : windowHandle(windowHandle), maximized(maximized) {}

        virtual bool persistent() override { return false; }
        virtual bool unique() override { return false; }
    };

    struct window_framebuffer_resize final : public events::event<window_framebuffer_resize>
    {
        ecs::component_handle<window> windowHandle;
        math::ivec2 size;

        window_framebuffer_resize() = default;
        window_framebuffer_resize(ecs::component_handle<window> windowHandle, math::ivec2 size) : windowHandle(windowHandle), size(size) {}

        virtual bool persistent() override { return false; }
        virtual bool unique() override { return false; }
    };

    struct window_content_rescale final : public events::event<window_content_rescale>
    {
        ecs::component_handle<window> windowHandle;
        math::vec2 scale;

        window_content_rescale() = default;
        window_content_rescale(ecs::component_handle<window> windowHandle, math::vec2 scale) : windowHandle(windowHandle), scale(scale) {}

        virtual bool persistent() override { return false; }
        virtual bool unique() override { return false; }
    };

    struct window_item_dropped final : public events::event<window_item_dropped>
    {
        ecs::component_handle<window> windowHandle;
        std::vector<cstring> paths;

        window_item_dropped() = default;
        window_item_dropped(ecs::component_handle<window> windowHandle, int count, cstring* cpaths) : windowHandle(windowHandle)
        {
            for (int i = 0; i < count; i++)
                paths.push_back(cpaths[i]);
        }

        virtual bool persistent() override { return false; }
        virtual bool unique() override { return false; }
    };

    struct mouse_enter_window final : public events::event<mouse_enter_window>
    {
        ecs::component_handle<window> windowHandle;
        int entered;

        mouse_enter_window() = default;
        mouse_enter_window(ecs::component_handle<window> windowHandle, int entered) : windowHandle(windowHandle), entered(entered) {}

        virtual bool persistent() override { return false; }
        virtual bool unique() override { return false; }
    };
}
