#pragma once
#pragma once
#include <core/events/event.hpp>
#include <application/window/window.hpp>

namespace args::application
{
    struct key_input final : public events::event<key_input>
    {
        ecs::component_handle<window> windowHandle;
        int key; 
        int scancode; 
        int action; 
        int mods;

        key_input() = default;
        key_input(ecs::component_handle<window> windowHandle, int key, int scancode, int action, int mods) : windowHandle(windowHandle), key(key), scancode(scancode), action(action), mods(mods) {}

        virtual bool persistent() { return false; }
        virtual bool unique() { return false; }
    };

    struct char_input final : public events::event<char_input>
    {
        ecs::component_handle<window> windowHandle;
        char32_t character;

        char_input() = default;
        char_input(ecs::component_handle<window> windowHandle, char32_t character) : windowHandle(windowHandle), character(character) {}

        virtual bool persistent() { return false; }
        virtual bool unique() { return false; }
    };
}
