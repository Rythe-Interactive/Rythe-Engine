#pragma once
#include "application/events/inputevents.hpp"
#include "application/events/windowinputevents.hpp"
#include "core/engine/system.hpp"
#include <core/containers/iterator_tricks.hpp>

namespace args::application
{


    class InputSystem : public core::System<InputSystem>
    {
    public:
        static inline std::set<int> m_presentGamepads;


        void setup() override
        {
            bindToEvent<key_input, &InputSystem::onKey>();
            bindToEvent<mouse_moved, &InputSystem::onMouseMove>();
            bindToEvent<mouse_button, &InputSystem::onMouseButton>();
            bindToEvent<mouse_scrolled, &InputSystem::onMouseScrolled>();
            createProcess<&InputSystem::onUpdate>("Input");
            ContextHelper::setJoystickCallback(&InputSystem::onCheckGamepadPresence);


            //make sure the mappings match here!
            //ContextHelper::updateGamepadMappings("assets/conf/gamepad.conf");

        }


        template <class Event>
        void createBinding(inputmap::method k, float value = 1) {
            static_assert(std::is_base_of_v<input_action, Event> ||
                std::is_base_of_v<input_axis, Event>,
                "Event needs to either be an input_action or an input_axis");

            if constexpr (std::is_base_of_v<input_action, Event>)
            {
                //are we dealing with a family of methods ?
                if (inputmap::is_family(k))
                {
                    for (inputmap::method m : inputmap::get_family(k))
                    {
                        if (inputmap::is_key(m))
                        {
                            bindKeyToAction<Event>(m);
                        }
                        if (inputmap::is_axis(m))
                        {
                            bindAxisToAction<Event>(m, value);
                        }
                    }
                }
                else
                {
                    if (inputmap::is_key(k))
                    {
                        bindKeyToAction<Event>(k);
                    }
                    if (inputmap::is_axis(k))
                    {
                        bindAxisToAction<Event>(k, value);
                    }
                }
            }
            else
            {
                if (inputmap::is_family(k))
                {
                    for (inputmap::method m : inputmap::get_family(k))
                    {
                        if (inputmap::is_key(m))
                        {
                            bindKeyToAxis<Event>(m);
                        }
                        if (inputmap::is_axis(m))
                        {
                            bindAxisToAxis<Event>(m, value);
                        }
                    }
                }
                else
                {
                    if (inputmap::is_key(k))
                    {
                        bindKeyToAxis<Event>(k);
                    }
                    if (inputmap::is_axis(k))
                    {
                        bindAxisToAxis<Event>(k, value);
                    }
                }
            }
        }

        template<class Event>
        void removeBinding(inputmap::method met)
        {
            static_assert(std::is_base_of_v<input_action, Event> ||
                std::is_base_of_v<input_axis, Event>,
                "Event needs to either be an input_action or an input_axis");

            if constexpr (std::is_base_of_v<input_action, Event>)
            {

                //are we dealing with a family of methods ?
                if (inputmap::is_family(met))
                {
                    for (inputmap::method member : inputmap::get_family(met))
                    {
                        if (inputmap::is_key(member))
                        {
                            m_actions[member][typeHash<Event>()].clear();
                        }
                        if (inputmap::is_axis(member))
                        {
                            std::get<0>(m_axes[member][typeHash<Event>()]).clear();
                        }
                    }
                }
                else
                {
                    if (inputmap::is_key(met))
                    {
                        m_actions[met][typeHash<Event>()].clear();
                    }
                    if (inputmap::is_axis(met))
                    {
                        std::get<0>(m_axes[met][typeHash<Event>()]).clear();
                    }
                }
            }
            else
            {
                if (inputmap::is_family(met))
                {
                    for (inputmap::method member : inputmap::get_family(met))
                    {
                        if (inputmap::is_key(member))
                        {
                            m_actions[member][typeHash<Event>()].clear();
                        }
                        if (inputmap::is_axis(member))
                        {
                            std::get<0>(m_axes[member][typeHash<Event>()]).clear();
                        }
                    }
                }
                else
                {
                    if (inputmap::is_key(met))
                    {
                        m_actions[m][typeHash<Event>()].clear();
                    }
                    if (inputmap::is_axis(met))
                    {
                        std::get<0>(m_axes[met][typeHash<Event>()]).clear();
                    }
                }
            }
        }

    private:

        template<class Event>
        void bindKeyToAction(inputmap::method m)
        {
            m_actions[m][typeHash<Event>()] = delegate<void(bool, inputmap::modifier_keys, inputmap::method)>::create([this]
            (bool state, inputmap::modifier_keys mods, inputmap::method method)
                {
                    Event e;
                    e.set(state, mods, method);
                    this->raiseEvent<Event>(e);
                });
        }

        template<class Event>
        void bindKeyToAxis(inputmap::method m)
        {
            m_actions[m][typeHash<Event>()] = delegate<void(bool, inputmap::modifier_keys, inputmap::method)>::create([this]
            (bool state, inputmap::modifier_keys mods, inputmap::method method)
                {
                    Event e;
                    e.set(state ? 1.0f : 0.0f, mods, method); //convert key state false:true to float range 1-0
                    this->raiseEvent<Event>(e);
                });
        }

        template<class Event>
        void bindAxisToAction(inputmap::method m, float value)
        {
            m_axes[m][typeHash<Event>()] = std::make_tuple(
                delegate<void(float, inputmap::modifier_keys, inputmap::method)>::create([this]
                (float value, inputmap::modifier_keys mods, inputmap::method method)
                    {
                        Event e;
                        e.set(value > 0.05f || value < -0.05f, mods, method); //convert float range 0-1 to key state false:true
                        this->raiseEvent<Event>(e);
                    }
                ),
                value, inputmap::modifier_keys::NONE, m
                        );
        }

        template<class Event>
        void bindAxisToAxis(inputmap::method m, float value)
        {
            m_axes[m][typeHash<Event>()] = std::make_tuple(
                delegate<void(float, inputmap::modifier_keys, inputmap::method)>::create([this]
                (float value, inputmap::modifier_keys mods, inputmap::method method)
                    {
                        Event e;
                        e.set(value, mods, method);
                        this->raiseEvent<Event>(e);
                    }
                ),
                value, inputmap::modifier_keys::NONE, m
                        );
        }

        static void onCheckGamepadPresence(int jid,int event)
        {

            if(event == GLFW_CONNECTED)
                m_presentGamepads.insert(jid);
            else if (event == GLFW_DISCONNECTED)
                m_presentGamepads.erase(jid);
        }

        void onUpdate(time::time_span<fast_time> deltaTime)
        {
            (void)deltaTime;


            onJoystick();

            //update all axis with their current values

            for (auto inner_map : m_axes)
            {
                for (auto [surrogate, value, mods, method] : inner_map)
                {
                    surrogate(value, mods, method);
                }
            }
        }

        void matchGLFWAxisWithSignalAxis(const GLFWgamepadstate& state,inputmap::modifier_keys joystick,
                                         const std::size_t glfw,inputmap::method m)
        {
                const float value = state.axes[glfw];
                for (auto& axis : m_axes[m])
                {
                    std::get<1>(axis) = value;
                    std::get<2>(axis) = joystick;
                    std::get<3>(axis) = m;
                }
        }

        void onJoystick()
        {
            for (int glfw_joystick_id : m_presentGamepads)
            {
                using mods = inputmap::modifier_keys;
                using method = inputmap::method;

                GLFWgamepadstate state;
                if (!ContextHelper::getGamepadSate(glfw_joystick_id, &state)) continue;

                const auto joystick = mods::JOYSTICK0 + glfw_joystick_id;

                for (auto& action : m_actions[method::GAMEPAD_A])
                    action(state.buttons[GLFW_GAMEPAD_BUTTON_A], joystick, method::GAMEPAD_A);
                for (auto& action : m_actions[method::GAMEPAD_B])
                    action(state.buttons[GLFW_GAMEPAD_BUTTON_B], joystick, method::GAMEPAD_B);
                for (auto& action : m_actions[method::GAMEPAD_X])
                    action(state.buttons[GLFW_GAMEPAD_BUTTON_X], joystick, method::GAMEPAD_X);
                for (auto& action : m_actions[method::GAMEPAD_Y])
                    action(state.buttons[GLFW_GAMEPAD_BUTTON_Y], joystick, method::GAMEPAD_Y);
                for (auto& action : m_actions[method::GAMEPAD_LEFT_BUMPER])
                    action(state.buttons[GLFW_GAMEPAD_BUTTON_LEFT_BUMPER], joystick, method::GAMEPAD_LEFT_BUMPER);
                for (auto& action : m_actions[method::GAMEPAD_RIGHT_BUMPER])
                    action(state.buttons[GLFW_GAMEPAD_BUTTON_RIGHT_BUMPER], joystick, method::GAMEPAD_RIGHT_BUMPER);
                for (auto& action : m_actions[method::GAMEPAD_BACK])
                    action(state.buttons[GLFW_GAMEPAD_BUTTON_BACK], joystick, method::GAMEPAD_BACK);
                for (auto& action : m_actions[method::GAMEPAD_START])
                    action(state.buttons[GLFW_GAMEPAD_BUTTON_START], joystick, method::GAMEPAD_START);
                for (auto& action : m_actions[method::GAMEPAD_DPAD_UP])
                    action(state.buttons[GLFW_GAMEPAD_BUTTON_DPAD_UP], joystick, method::GAMEPAD_DPAD_UP);
                for (auto& action : m_actions[method::GAMEPAD_DPAD_RIGHT])
                    action(state.buttons[GLFW_GAMEPAD_BUTTON_DPAD_RIGHT], joystick, method::GAMEPAD_DPAD_RIGHT);
                for (auto& action : m_actions[method::GAMEPAD_DPAD_LEFT])
                    action(state.buttons[GLFW_GAMEPAD_BUTTON_DPAD_LEFT], joystick, method::GAMEPAD_DPAD_LEFT);
                for (auto& action : m_actions[method::GAMEPAD_DPAD_DOWN])
                    action(state.buttons[GLFW_GAMEPAD_BUTTON_DPAD_DOWN], joystick, method::GAMEPAD_DPAD_DOWN);
                for (auto& action : m_actions[method::GAMEPAD_LEFT_THUMB]) 
                    action(state.buttons[GLFW_GAMEPAD_BUTTON_LEFT_THUMB],joystick,method::GAMEPAD_LEFT_THUMB);
                for (auto& action : m_actions[method::GAMEPAD_RIGHT_THUMB]) 
                    action(state.buttons[GLFW_GAMEPAD_BUTTON_RIGHT_THUMB],joystick,method::GAMEPAD_RIGHT_THUMB);

                matchGLFWAxisWithSignalAxis(state,joystick,GLFW_GAMEPAD_AXIS_LEFT_X,method::GAMEPAD_LEFT_X);
                matchGLFWAxisWithSignalAxis(state,joystick,GLFW_GAMEPAD_AXIS_LEFT_Y,method::GAMEPAD_LEFT_Y);
                matchGLFWAxisWithSignalAxis(state,joystick,GLFW_GAMEPAD_AXIS_LEFT_TRIGGER,method::GAMEPAD_LEFT_TRIGGER);
                matchGLFWAxisWithSignalAxis(state,joystick,GLFW_GAMEPAD_AXIS_RIGHT_X,method::GAMEPAD_RIGHT_X);
                matchGLFWAxisWithSignalAxis(state,joystick,GLFW_GAMEPAD_AXIS_RIGHT_Y,method::GAMEPAD_RIGHT_Y);
                matchGLFWAxisWithSignalAxis(state,joystick,GLFW_GAMEPAD_AXIS_RIGHT_TRIGGER,method::GAMEPAD_RIGHT_TRIGGER);
            }
        }

        static inputmap::modifier_keys translateModifierKeys(int glfw_mods)
        {
            using mods = inputmap::modifier_keys;
            mods result = mods::NONE;

            if(glfw_mods & GLFW_MOD_ALT)
                result = static_cast<mods>(result & inputmap::modifier_keys::ALT);

            if(glfw_mods & GLFW_MOD_SHIFT)
                result = static_cast<mods>(result & inputmap::modifier_keys::SHIFT);

            if(glfw_mods & GLFW_MOD_CONTROL)
                result = static_cast<mods>(result & inputmap::modifier_keys::CTRL);

            return result;
        }

        void onKey(key_input* window_key_event)
        {
            const auto m = static_cast<inputmap::method>(window_key_event->key);
            for(auto& action : m_actions[m])
            {
                action(window_key_event->action==GLFW_PRESS,translateModifierKeys(window_key_event->mods),m);
            }
        }

        void onMouseMove(mouse_moved* window_mouse_event)
        {

        }

        void onMouseButton(mouse_button* window_mouse_event)
        {

        }

        void onMouseScrolled(mouse_scrolled* window_mouse_event)
        {

        }


        
        sparse_map<inputmap::method, sparse_map<id_type, delegate<void(bool, inputmap::modifier_keys, inputmap::method)>>> m_actions;
        sparse_map<inputmap::method, sparse_map<id_type, std::tuple<delegate<void(float, inputmap::modifier_keys, inputmap::method)>, float, inputmap::modifier_keys, inputmap::method>>> m_axes;

    };
}
