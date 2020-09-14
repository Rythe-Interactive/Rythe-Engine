#pragma once
#ifndef ARGS_IMPORT
#define ARGS_IMPORT
#include <core/core.hpp>
#include <core/platform/args_library.hpp>
#else
#include <core/core.hpp>
#endif // !ARGS_IMPORT

#include <application/input/inputmap.hpp>


namespace args::application
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

        virtual bool unique() override { return false; }

        A_NODISCARD bool pressed()  const { return value == true;  }
        A_NODISCARD bool released() const { return value == false; }
    };

    template <class T>
    struct input_axis : public events::event<T>
    {
        void set(float v,inputmap::modifier_keys m,inputmap::method id)
        {
            value = v;
            mods = m;
            identifier = id;
        }

        float input_delta;
        float value{};
        inputmap::modifier_keys mods{};
        inputmap::method identifier{};

        virtual bool unique() override { return false; }
    };

}
