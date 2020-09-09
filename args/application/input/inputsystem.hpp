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
        void setup() override
        {
            //subscribe to the raw events emitted from the window system
            bindToEvent<key_input, &InputSystem::onKey>();
            bindToEvent<mouse_moved, &InputSystem::onMouseMove>();
            bindToEvent<mouse_button, &InputSystem::onMouseButton>();
            bindToEvent<mouse_scrolled, &InputSystem::onMouseScrolled>();

            //create Update Process
            createProcess<&InputSystem::onUpdate>("Input");


            //make sure we get the joystick-callback on initialization of GLFW
            ContextHelper::addOnInitCallback(delegate<void()>::create([]
                {
                    ContextHelper::setJoystickCallback(&InputSystem::onCheckGamepadPresence);

                    //also enumerate Joysticks at the beginning of the Engine, the callback is not called on Joysticks that
                    //that are already connected

                    //note that GLFW only supports 16 gamepads!
                    for (size_t i = 0; i < inputmap::modifier_keys::MAX_SIZE - inputmap::modifier_keys::JOYSTICK0; ++i)
                    {
                        if (ContextHelper::joystickPresent(i))
                        {
                            m_presentGamepads.insert(i);
                        }
                    }
                }));
            //make sure the mappings match here!
            //ContextHelper::updateGamepadMappings("assets/conf/gamepad.conf");

        }

        /**
         * @brief Creates a Binding of a Key /Axis to the emission of an event in the event bus.
         *
         * @tparam Event The type of event you want to be emitted on the appearance of the action/axis.
         * @param k The axis/action you want to listen to. @see inputmap::method for options
         * @param value The value you want to map to if your Event is an Axis but the event is a button/key.
         *         For instance if you want to map one axis to 'W/S' you can make 'W' emit 1 and 'S' -1.
         */
        template <class Event>
        static void createBinding(inputmap::method k, float value = 1) {
            static_assert(std::is_base_of_v<input_action<Event>, Event> ||
                std::is_base_of_v<input_axis<Event>, Event>,
                "Event needs to either be an input_action or an input_axis");

            if constexpr (std::is_base_of_v<input_action<Event>, Event>)
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
                            bindKeyToAxis<Event>(m, value);
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
                        bindKeyToAxis<Event>(k, value);
                    }
                    if (inputmap::is_axis(k))
                    {
                        bindAxisToAxis<Event>(k, value);
                    }
                }
            }
        }

        /** @brief removes a Binding from a method
         *  @tparam Event the Event you want to unbind
         *  @param met the method you want to unbind from
         */
        template<class Event>
        static void removeBinding(inputmap::method met)
        {
            static_assert(std::is_base_of_v<input_action<Event>, Event> ||
                std::is_base_of_v<input_axis<Event>, Event>,
                "Event needs to either be an input_action or an input_axis");

            if constexpr (std::is_base_of_v<input_action<Event>, Event>)
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
                        m_actions[met][typeHash<Event>()].clear();
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
        static void bindKeyToAction(inputmap::method m)
        {
            //creates a tuple with default value 0
            m_actions[m][typeHash<Event>()] = std::make_tuple(
                delegate<void(InputSystem*, bool, inputmap::modifier_keys, inputmap::method, float)>::create([]
                (InputSystem* self, bool state, inputmap::modifier_keys mods, inputmap::method method, float def)
                    {
                        (void)def;
                        Event e;
                        e.set(state, mods, method);
                        self->raiseEvent<Event>(e);
                    }),
                0
                        );
        }

        template<class Event>
        static void bindKeyToAxis(inputmap::method m, float value)
        {
            //creates tuple embedding `value`
            m_actions[m][typeHash<Event>()] = std::make_tuple(
                delegate<void(InputSystem*, bool, inputmap::modifier_keys, inputmap::method, float)>::create([]
                (InputSystem* self, bool state, inputmap::modifier_keys mods, inputmap::method method, float def)
                    {
                        Event e;
                        e.set(state ? def : 0.0f, mods, method); //convert key state false:true to float range 1-0
                        self->raiseEvent<Event>(e);
                    }),
                value
                        );
        }

        template<class Event>
        static void bindAxisToAction(inputmap::method m, float value)
        {
            //creates tuple embedding all parameters needed for invoking the action
            m_axes[m][typeHash<Event>()] = std::make_tuple(
                delegate<void(InputSystem*, float, inputmap::modifier_keys, inputmap::method)>::create([]
                (InputSystem* self, float value, inputmap::modifier_keys mods, inputmap::method method)
                    {
                        Event e;
                        e.set(value > 0.05f || value < -0.05f, mods, method); //convert float range 0-1 to key state false:true
                        self->raiseEvent<Event>(e);
                    }
                ),
                value, inputmap::modifier_keys::NONE, m
                        );
        }

        template<class Event>
        static void bindAxisToAxis(inputmap::method m, float value)
        {
            //creates tuple embedding all parameters needed for invoking the action
            m_axes[m][typeHash<Event>()] = std::make_tuple(
                delegate<void(InputSystem*, float, inputmap::modifier_keys, inputmap::method)>::create([]
                (InputSystem* self, float value, inputmap::modifier_keys mods, inputmap::method method)
                    {
                        Event e;
                        e.set(value, mods, method);
                        self->raiseEvent<Event>(e);
                    }
                ),
                value, inputmap::modifier_keys::NONE, m
                        );
        }

        //joystick (dis)connect callback
        static void onCheckGamepadPresence(int jid, int event)
        {

            if (event == GLFW_CONNECTED)
                m_presentGamepads.insert(jid);
            else if (event == GLFW_DISCONNECTED)
                m_presentGamepads.erase(jid);
        }

        void onUpdate(time::time_span<fast_time> deltaTime)
        {
            (void)deltaTime;


            onJoystick();

            //update all axis with their current values

            for (auto& inner_map : m_axes)
            {
                for (auto& [surrogate, value, mods, method] : inner_map)
                {
                    surrogate(this, value, mods, method);
                }
            }
        }

        void matchGLFWAxisWithSignalAxis(const GLFWgamepadstate& state, inputmap::modifier_keys joystick,
            const std::size_t glfw, inputmap::method m)
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

                for (auto& [action, def] : m_actions[method::GAMEPAD_A])
                    action(this, state.buttons[GLFW_GAMEPAD_BUTTON_A], joystick, method::GAMEPAD_A, def);
                for (auto& [action, def] : m_actions[method::GAMEPAD_B])
                    action(this, state.buttons[GLFW_GAMEPAD_BUTTON_B], joystick, method::GAMEPAD_B, def);
                for (auto& [action, def] : m_actions[method::GAMEPAD_X])
                    action(this, state.buttons[GLFW_GAMEPAD_BUTTON_X], joystick, method::GAMEPAD_X, def);
                for (auto& [action, def] : m_actions[method::GAMEPAD_Y])
                    action(this, state.buttons[GLFW_GAMEPAD_BUTTON_Y], joystick, method::GAMEPAD_Y, def);
                for (auto& [action, def] : m_actions[method::GAMEPAD_LEFT_BUMPER])
                    action(this, state.buttons[GLFW_GAMEPAD_BUTTON_LEFT_BUMPER], joystick, method::GAMEPAD_LEFT_BUMPER, def);
                for (auto& [action, def] : m_actions[method::GAMEPAD_RIGHT_BUMPER])
                    action(this, state.buttons[GLFW_GAMEPAD_BUTTON_RIGHT_BUMPER], joystick, method::GAMEPAD_RIGHT_BUMPER, def);
                for (auto& [action, def] : m_actions[method::GAMEPAD_BACK])
                    action(this, state.buttons[GLFW_GAMEPAD_BUTTON_BACK], joystick, method::GAMEPAD_BACK, def);
                for (auto& [action, def] : m_actions[method::GAMEPAD_START])
                    action(this, state.buttons[GLFW_GAMEPAD_BUTTON_START], joystick, method::GAMEPAD_START, def);
                for (auto& [action, def] : m_actions[method::GAMEPAD_DPAD_UP])
                    action(this, state.buttons[GLFW_GAMEPAD_BUTTON_DPAD_UP], joystick, method::GAMEPAD_DPAD_UP, def);
                for (auto& [action, def] : m_actions[method::GAMEPAD_DPAD_RIGHT])
                    action(this, state.buttons[GLFW_GAMEPAD_BUTTON_DPAD_RIGHT], joystick, method::GAMEPAD_DPAD_RIGHT, def);
                for (auto& [action, def] : m_actions[method::GAMEPAD_DPAD_LEFT])
                    action(this, state.buttons[GLFW_GAMEPAD_BUTTON_DPAD_LEFT], joystick, method::GAMEPAD_DPAD_LEFT, def);
                for (auto& [action, def] : m_actions[method::GAMEPAD_DPAD_DOWN])
                    action(this, state.buttons[GLFW_GAMEPAD_BUTTON_DPAD_DOWN], joystick, method::GAMEPAD_DPAD_DOWN, def);
                for (auto& [action, def] : m_actions[method::GAMEPAD_LEFT_THUMB])
                    action(this, state.buttons[GLFW_GAMEPAD_BUTTON_LEFT_THUMB], joystick, method::GAMEPAD_LEFT_THUMB, def);
                for (auto& [action, def] : m_actions[method::GAMEPAD_RIGHT_THUMB])
                    action(this, state.buttons[GLFW_GAMEPAD_BUTTON_RIGHT_THUMB], joystick, method::GAMEPAD_RIGHT_THUMB, def);

                matchGLFWAxisWithSignalAxis(state, joystick, GLFW_GAMEPAD_AXIS_LEFT_X, method::GAMEPAD_LEFT_X);
                matchGLFWAxisWithSignalAxis(state, joystick, GLFW_GAMEPAD_AXIS_LEFT_Y, method::GAMEPAD_LEFT_Y);
                matchGLFWAxisWithSignalAxis(state, joystick, GLFW_GAMEPAD_AXIS_LEFT_TRIGGER, method::GAMEPAD_LEFT_TRIGGER);
                matchGLFWAxisWithSignalAxis(state, joystick, GLFW_GAMEPAD_AXIS_RIGHT_X, method::GAMEPAD_RIGHT_X);
                matchGLFWAxisWithSignalAxis(state, joystick, GLFW_GAMEPAD_AXIS_RIGHT_Y, method::GAMEPAD_RIGHT_Y);
                matchGLFWAxisWithSignalAxis(state, joystick, GLFW_GAMEPAD_AXIS_RIGHT_TRIGGER, method::GAMEPAD_RIGHT_TRIGGER);
            }
        }

        static inputmap::modifier_keys translateModifierKeys(int glfw_mods)
        {
            using mods = inputmap::modifier_keys;
            mods result = mods::NONE;

            if (glfw_mods & GLFW_MOD_ALT)
                result = static_cast<mods>(result & inputmap::modifier_keys::ALT);

            if (glfw_mods & GLFW_MOD_SHIFT)
                result = static_cast<mods>(result & inputmap::modifier_keys::SHIFT);

            if (glfw_mods & GLFW_MOD_CONTROL)
                result = static_cast<mods>(result & inputmap::modifier_keys::CTRL);

            return result;
        }

        void onKey(key_input* window_key_event)
        {
            log::debug("{}", window_key_event->key);
            const auto m = static_cast<inputmap::method>(window_key_event->key);
            for (auto& [action, def] : m_actions[m])
            {
                action(this, window_key_event->action != GLFW_RELEASE, translateModifierKeys(window_key_event->mods), m, def);
            }
        }

        void onMouseMove(mouse_moved* window_mouse_event)
        {
            auto pos = window_mouse_event->position;
            for (auto& axis : m_axes[inputmap::method::MOUSE_X])
            {
                std::get<1>(axis) = static_cast<float>(pos.x);
                std::get<2>(axis) = inputmap::modifier_keys::NONE;
                std::get<3>(axis) = inputmap::method::MOUSE_X;
            }
            for (auto& axis : m_axes[inputmap::method::MOUSE_Y])
            {
                std::get<1>(axis) = static_cast<float>(pos.y);
                std::get<2>(axis) = inputmap::modifier_keys::NONE;
                std::get<3>(axis) = inputmap::method::MOUSE_Y;
            }
        }

        void onMouseButton(mouse_button* window_mouse_event)
        {
            switch (window_mouse_event->button)
            {
            case GLFW_MOUSE_BUTTON_LEFT: {
                for (auto& [action, def] : m_actions[inputmap::method::MOUSE_LEFT])
                {
                    action(this, window_mouse_event->action != GLFW_RELEASE, translateModifierKeys(window_mouse_event->mods),
                        inputmap::method::MOUSE_LEFT, def);
                }
                break;
            }
            case GLFW_MOUSE_BUTTON_MIDDLE: {
                for (auto& [action, def] : m_actions[inputmap::method::MOUSE_MIDDLE])
                {
                    action(this, window_mouse_event->action != GLFW_RELEASE, translateModifierKeys(window_mouse_event->mods),
                        inputmap::method::MOUSE_MIDDLE, def);
                }
                break;
            }
            case GLFW_MOUSE_BUTTON_RIGHT: {
                for (auto& [action, def] : m_actions[inputmap::method::MOUSE_RIGHT])
                {
                    action(this, window_mouse_event->action != GLFW_RELEASE, translateModifierKeys(window_mouse_event->mods),
                        inputmap::method::MOUSE_RIGHT, def);
                }
                break;
            }
            }
        }

        void onMouseScrolled(mouse_scrolled* window_mouse_event)
        {
            const auto pos = window_mouse_event->offset;
            for (auto& axis : m_axes[inputmap::method::HSCROLL])
            {
                std::get<1>(axis) += static_cast<float>(pos.x);
                std::get<2>(axis) = inputmap::modifier_keys::NONE;
                std::get<3>(axis) = inputmap::method::HSCROLL;
            }
            for (auto& axis : m_axes[inputmap::method::VSCROLL])
            {
                std::get<1>(axis) += static_cast<float>(pos.y);
                std::get<2>(axis) = inputmap::modifier_keys::NONE;
                std::get<3>(axis) = inputmap::method::VSCROLL;
            }
        }


        inline static std::set<int> m_presentGamepads;
        inline static sparse_map<inputmap::method, sparse_map<id_type,
            std::tuple<delegate<void(InputSystem*, bool, inputmap::modifier_keys, inputmap::method, float)>, float>>
            > m_actions;

        inline static sparse_map<inputmap::method, sparse_map<id_type,
            std::tuple<delegate<void(InputSystem*, float, inputmap::modifier_keys, inputmap::method)>, float, inputmap::modifier_keys, inputmap::method>>
            > m_axes;

    };
}
