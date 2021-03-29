#pragma once
#pragma once
#include <core/events/event.hpp>
#include <application/window/window.hpp>

namespace legion::application
{
    struct key_input final : public events::event<key_input>
    {
        ecs::component<window> windowHandle;
        int key; 
        int scancode; 
        int action; 
        int mods;

        key_input() = default;
        key_input(ecs::component<window> windowHandle, int key, int scancode, int action, int mods) : windowHandle(windowHandle), key(key), scancode(scancode), action(action), mods(mods) {}

        
    };

    struct char_input final : public events::event<char_input>
    {
        ecs::component<window> windowHandle;
        char32_t character;

        char_input() = default;
        char_input(ecs::component<window> windowHandle, char32_t character) : windowHandle(windowHandle), character(character) {}

        
    };

    struct mouse_moved final : public events::event<mouse_moved>
    {
        ecs::component<window> windowHandle;
        math::dvec2 position;

        mouse_moved() = default;
        mouse_moved(ecs::component<window> windowHandle, math::dvec2 position) : windowHandle(windowHandle), position(position) {}

        
    };

    struct mouse_button final : public events::event<mouse_button>
    {
        ecs::component<window> windowHandle;
        int button;
        int action;
        int mods;

        mouse_button() = default;
        mouse_button(ecs::component<window> windowHandle, int button, int action, int mods) : windowHandle(windowHandle), button(button), action(action), mods(mods) {}

        
    };

    struct mouse_scrolled final : public events::event<mouse_scrolled>
    {
        ecs::component<window> windowHandle;
        math::dvec2 offset;

        mouse_scrolled() = default;
        mouse_scrolled(ecs::component<window> windowHandle, math::dvec2 offset) : windowHandle(windowHandle), offset(offset) {}

        
    };

}
