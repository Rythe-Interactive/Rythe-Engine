#pragma once
#include <core/core.hpp>
#include <application/input/inputmap.hpp>


namespace legion::application
{
    template <class T>
    struct input_action : public events::event<T>
    {

        void set(bool v, inputmap::modifier_keys m, inputmap::method id)
        {
            value = v;
            mods = m;
            identifier = id;
        }

        float input_delta;
        bool value{};
        inputmap::modifier_keys mods{};
        inputmap::method identifier{};

        L_NODISCARD bool pressed()  const { return value == true;  }
        L_NODISCARD bool released() const { return value == false; }
    };

    template <class T>
    struct input_axis : public events::event<T>
    {
        float input_delta;

        std::vector<float> value_parts;
        float value{};

        std::vector<inputmap::modifier_keys> mods_parts{};
        inputmap::modifier_keys mods{};

        std::vector<inputmap::method> identifier_parts{};
        inputmap::method identifier{};
    };

}
