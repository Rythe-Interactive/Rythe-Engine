#pragma once
#include <set>
#include <cstdint>
#include <cstddef>

namespace legion::application
{
    struct inputmap
    {
        enum class modifier_keys : math::uint8_t
        {
            NONE = 0,
            CTRL = 1,
            ALT = 2,
            SHIFT = 4,

            JOYSTICK0 = 5,
            JOYSTICK1 = 6,
            JOYSTICK2 = 7,
            JOYSTICK3 = 8,
            MAX_SIZE = 16 + JOYSTICK0
        };

        static math::uint8_t value(modifier_keys m)
        {
            return static_cast<math::uint8_t>(m);
        }

        friend math::uint8_t operator-(modifier_keys lhs, modifier_keys rhs)
        {
            return value(lhs) - value(rhs);
        }

        friend math::uint8_t operator+(modifier_keys lhs, modifier_keys rhs)
        {
            return value(lhs) + value(rhs);
        }
        friend modifier_keys operator+(modifier_keys lhs, math::uint8_t rhs)
        {
            return static_cast<modifier_keys>(value(lhs) + rhs);
        }
        friend math::uint8_t operator&(modifier_keys lhs, modifier_keys rhs)
        {
            return value(lhs) & value(rhs);
        }
        friend math::uint8_t operator|(modifier_keys lhs, modifier_keys rhs)
        {
            return value(lhs) | value(rhs);
        }

        enum class method
        {
            KEY,
            CHAR,
            MOUSE,
            JOY,
            SCROLL,

            GAMEPAD_A,
            GAMEPAD_B,
            GAMEPAD_X,
            GAMEPAD_Y,
            GAMEPAD_LEFT_BUMPER,
            GAMEPAD_RIGHT_BUMPER,
            GAMEPAD_BACK,
            GAMEPAD_START,
            GAMEPAD_GUIDE,
            GAMEPAD_LEFT_AXIS,
            GAMEPAD_RIGHT_AXIS,
            GAMEPAD_LEFT_THUMB,
            GAMEPAD_RIGHT_THUMB,
            GAMEPAD_DPAD_UP,
            GAMEPAD_DPAD_RIGHT,
            GAMEPAD_DPAD_DOWN,
            GAMEPAD_DPAD_LEFT,

            GAMEPAD_LEFT_X,
            GAMEPAD_LEFT_Y,
            GAMEPAD_RIGHT_X,
            GAMEPAD_RIGHT_Y,
            GAMEPAD_LEFT_TRIGGER,
            GAMEPAD_RIGHT_TRIGGER,

            /* Printable keys */
            SPACE = 32,
            APOSTROPHE = 39,
            COMMA = 44,
            MINUS = 45,
            PERIOD = 46,
            SLASH = 47,
            NUM0 = 48,
            NUM1 = 49,
            NUM2 = 50,
            NUM3 = 51,
            NUM4 = 52,
            NUM5 = 53,
            NUM6 = 54,
            NUM7 = 55,
            NUM8 = 56,
            NUM9 = 57,
            SEMICOLON = 59,
            EQUAL = 61,
            A = 65,
            B = 66,
            C = 67,
            D = 68,
            E = 69,
            F = 70,
            G = 71,
            H = 72,
            I = 73,
            J = 74,
            K = 75,
            L = 76,
            M = 77,
            N = 78,
            O = 79,
            P = 80,
            Q = 81,
            R = 82,
            S = 83,
            T = 84,
            U = 85,
            V = 86,
            W = 87,
            X = 88,
            Y = 89,
            Z = 90,
            LEFT_BRACKET = 91,
            BACKSLASH = 92,
            RIGHT_BRACKET = 93,
            GRAVE_ACCENT = 96,
            WORLD_1 = 161,
            WORLD_2 = 162,

            /* Function keys */
            ESCAPE = 256,
            ENTER = 257,
            TAB = 258,
            BACKSPACE = 259,
            INSERT = 260,
            DEL = 261,
            RIGHT = 262,
            LEFT = 263,
            DOWN = 264,
            UP = 265,
            PAGE_UP = 266,
            PAGE_DOWN = 267,
            HOME = 268,
            END = 269,
            CAPS_LOCK = 280,
            SCROLL_LOCK = 281,
            NUM_LOCK = 282,
            PRINT_SCREEN = 283,
            PAUSE = 284,
            F1 = 290,
            F2 = 291,
            F3 = 292,
            F4 = 293,
            F5 = 294,
            F6 = 295,
            F7 = 296,
            F8 = 297,
            F9 = 298,
            F10 = 299,
            F11 = 300,
            F12 = 301,
            F13 = 302,
            F14 = 303,
            F15 = 304,
            F16 = 305,
            F17 = 306,
            F18 = 307,
            F19 = 308,
            F20 = 309,
            F21 = 310,
            F22 = 311,
            F23 = 312,
            F24 = 313,
            F25 = 314,
            KP_0 = 320,
            KP_1 = 321,
            KP_2 = 322,
            KP_3 = 323,
            KP_4 = 324,
            KP_5 = 325,
            KP_6 = 326,
            KP_7 = 327,
            KP_8 = 328,
            KP_9 = 329,
            KP_DECIMAL = 330,
            KP_DIVIDE = 331,
            KP_MULTIPLY = 332,
            KP_SUBTRACT = 333,
            KP_ADD = 334,
            KP_ENTER = 335,
            KP_EQUAL = 336,
            LEFT_SHIFT = 340,
            LEFT_CONTROL = 341,
            LEFT_ALT = 342,
            LEFT_SUPER = 343,
            RIGHT_SHIFT = 344,
            RIGHT_CONTROL = 345,
            RIGHT_ALT = 346,
            RIGHT_SUPER = 347,
            MENU = 348,

            HSCROLL,
            VSCROLL,

            MOUSE_LEFT,
            MOUSE_MIDDLE,
            MOUSE_RIGHT,
            MOUSE_X,
            MOUSE_Y

        };


        static constexpr bool is_axis(method m)
        {
            return
                m == method::MOUSE || m == method::JOY || m == method::SCROLL || m == method::GAMEPAD_LEFT_TRIGGER ||
                m == method::GAMEPAD_RIGHT_TRIGGER || m == method::GAMEPAD_LEFT_AXIS || m == method::GAMEPAD_RIGHT_AXIS ||
                m == method::GAMEPAD_LEFT_X || m == method::GAMEPAD_LEFT_Y || m == method::GAMEPAD_RIGHT_X ||
                m == method::GAMEPAD_RIGHT_Y || m == method::HSCROLL || m == method::VSCROLL ||
                m == method::MOUSE_X || m == method::MOUSE_Y;
        }

        static constexpr bool is_key(method m)
        {
            return
                m == method::KEY || m == method::CHAR || m == method::JOY || m == method::MOUSE || m == method::GAMEPAD_A ||
                m == method::GAMEPAD_B || m == method::GAMEPAD_X || m == method::GAMEPAD_Y ||
                m == method::GAMEPAD_LEFT_BUMPER || m == method::GAMEPAD_RIGHT_BUMPER || m == method::GAMEPAD_BACK ||
                m == method::GAMEPAD_START || m == method::GAMEPAD_GUIDE || m == method::GAMEPAD_DPAD_UP ||
                m == method::GAMEPAD_DPAD_RIGHT || m == method::GAMEPAD_DPAD_DOWN || m == method::GAMEPAD_DPAD_LEFT ||
                m == method::SPACE || m == method::APOSTROPHE || m == method::COMMA || m == method::MINUS ||
                m == method::PERIOD || m == method::SLASH || m == method::NUM0 || m == method::NUM1 ||
                m == method::NUM2 || m == method::NUM3 || m == method::NUM4 || m == method::NUM5 || m == method::NUM6 ||
                m == method::NUM7 || m == method::NUM8 || m == method::NUM9 || m == method::SEMICOLON ||
                m == method::EQUAL || m == method::A || m == method::B || m == method::C || m == method::D ||
                m == method::E || m == method::F || m == method::G || m == method::H || m == method::I || m == method::J ||
                m == method::K || m == method::L || m == method::M || m == method::N || m == method::O || m == method::P ||
                m == method::Q || m == method::R || m == method::S || m == method::T || m == method::U || m == method::V ||
                m == method::W || m == method::X || m == method::Y || m == method::Z || m == method::LEFT_BRACKET ||
                m == method::BACKSLASH || m == method::RIGHT_BRACKET || m == method::GRAVE_ACCENT || m == method::WORLD_1 ||
                m == method::WORLD_2 || m == method::ESCAPE || m == method::ENTER || m == method::TAB ||
                m == method::BACKSPACE || m == method::INSERT || m == method::DEL || m == method::RIGHT ||
                m == method::LEFT || m == method::DOWN || m == method::UP || m == method::PAGE_UP ||
                m == method::PAGE_DOWN || m == method::HOME || m == method::END || m == method::CAPS_LOCK ||
                m == method::SCROLL_LOCK || m == method::NUM_LOCK || m == method::PRINT_SCREEN || m == method::PAUSE ||
                m == method::F1 || m == method::F2 || m == method::F3 || m == method::F4 || m == method::F5 ||
                m == method::F6 || m == method::F7 || m == method::F8 || m == method::F9 || m == method::F10 ||
                m == method::F11 || m == method::F12 || m == method::F13 || m == method::F14 || m == method::F15 ||
                m == method::F16 || m == method::F17 || m == method::F18 || m == method::F19 || m == method::F20 ||
                m == method::F21 || m == method::F22 || m == method::F23 || m == method::F24 || m == method::F25 ||
                m == method::KP_0 || m == method::KP_1 || m == method::KP_2 || m == method::KP_3 || m == method::KP_4 ||
                m == method::KP_5 || m == method::KP_6 || m == method::KP_7 || m == method::KP_8 || m == method::KP_9 ||
                m == method::KP_DECIMAL || m == method::KP_DIVIDE || m == method::KP_MULTIPLY || m == method::KP_SUBTRACT ||
                m == method::KP_ADD || m == method::KP_ENTER || m == method::KP_EQUAL || m == method::LEFT_SHIFT ||
                m == method::LEFT_CONTROL || m == method::LEFT_ALT || m == method::LEFT_SUPER || m == method::RIGHT_SHIFT ||
                m == method::RIGHT_CONTROL || m == method::RIGHT_ALT || m == method::RIGHT_SUPER || m == method::MENU ||
                m == method::MOUSE_LEFT || m == method::MOUSE_MIDDLE || m == method::MOUSE_RIGHT ||
                m == method::GAMEPAD_RIGHT_THUMB || m == method::GAMEPAD_LEFT_THUMB;
        }

        static constexpr bool has_keys_and_axis(method m)
        {
            return is_key(m) && is_axis(m);
        }

        static constexpr bool is_family(method m)
        {
            return m == method::KEY || m == method::JOY || m == method::MOUSE || m == method::SCROLL ||
                m == method::GAMEPAD_RIGHT_AXIS || m == method::GAMEPAD_LEFT_AXIS;
        }

        static std::set<method> get_family(method family)
        {

            switch (family) {
            case method::KEY:
            {
                return std::set<method>{
                    method::SPACE, method::APOSTROPHE, method::COMMA, method::MINUS, method::PERIOD, method::SLASH,
                        method::NUM0, method::NUM1, method::NUM2, method::NUM3, method::NUM4, method::NUM5, method::NUM6,
                        method::NUM7, method::NUM8, method::NUM9, method::SEMICOLON, method::EQUAL, method::A, method::B,
                        method::C, method::D, method::E, method::F, method::G, method::H, method::I, method::J, method::K,
                        method::L, method::M, method::N, method::O, method::P, method::Q, method::R, method::S, method::T,
                        method::U, method::V, method::W, method::X, method::Y, method::Z, method::LEFT_BRACKET,
                        method::BACKSLASH, method::RIGHT_BRACKET, method::GRAVE_ACCENT, method::WORLD_1, method::WORLD_2,
                        method::ESCAPE, method::ENTER, method::TAB, method::BACKSPACE, method::INSERT, method::DEL,
                        method::RIGHT, method::LEFT, method::DOWN, method::UP, method::PAGE_UP, method::PAGE_DOWN,
                        method::HOME, method::END, method::CAPS_LOCK, method::SCROLL_LOCK, method::NUM_LOCK,
                        method::PRINT_SCREEN, method::PAUSE, method::F1, method::F2, method::F3, method::F4, method::F5,
                        method::F6, method::F7, method::F8, method::F9, method::F10, method::F11, method::F12, method::F13,
                        method::F14, method::F15, method::F16, method::F17, method::F18, method::F19, method::F20,
                        method::F21, method::F22, method::F23, method::F24, method::F25, method::KP_0, method::KP_1,
                        method::KP_2, method::KP_3, method::KP_4, method::KP_5, method::KP_6, method::KP_7, method::KP_8,
                        method::KP_9, method::KP_DECIMAL, method::KP_DIVIDE, method::KP_MULTIPLY, method::KP_SUBTRACT,
                        method::KP_ADD, method::KP_ENTER, method::KP_EQUAL, method::LEFT_SHIFT, method::LEFT_CONTROL,
                        method::LEFT_ALT, method::LEFT_SUPER, method::RIGHT_SHIFT, method::RIGHT_CONTROL, method::RIGHT_ALT,
                        method::RIGHT_SUPER, method::MENU,
                };
            }
            case  method::JOY:
            {
                return std::set<method>{
                    method::GAMEPAD_A, method::GAMEPAD_B, method::GAMEPAD_X, method::GAMEPAD_Y, method::GAMEPAD_LEFT_BUMPER,
                        method::GAMEPAD_RIGHT_BUMPER, method::GAMEPAD_BACK, method::GAMEPAD_START, method::GAMEPAD_GUIDE,
                        method::GAMEPAD_DPAD_UP, method::GAMEPAD_DPAD_RIGHT,
                        method::GAMEPAD_DPAD_DOWN, method::GAMEPAD_DPAD_LEFT, method::GAMEPAD_LEFT_X, method::GAMEPAD_LEFT_Y,
                        method::GAMEPAD_RIGHT_X, method::GAMEPAD_RIGHT_Y, method::GAMEPAD_LEFT_TRIGGER, method::GAMEPAD_RIGHT_TRIGGER,
                        method::GAMEPAD_RIGHT_THUMB, method::GAMEPAD_LEFT_THUMB

                };
            }
            case method::GAMEPAD_LEFT_AXIS:
            {
                return std::set<method>{
                    method::GAMEPAD_LEFT_X, method::GAMEPAD_LEFT_Y
                };
            }
            case method::GAMEPAD_RIGHT_AXIS:
            {
                return std::set<method> {
                    method::GAMEPAD_RIGHT_X, method::GAMEPAD_RIGHT_Y
                };
            }
            case method::MOUSE:
            {
                return std::set<method>{method::MOUSE_Y, method::MOUSE_X, method::MOUSE_LEFT, method::MOUSE_MIDDLE, method::MOUSE_RIGHT};
            }

            case method::SCROLL:
            {
                return std::set<method>{method::HSCROLL, method::VSCROLL};
            }
            default: return {};
            }
        }
    };
}
