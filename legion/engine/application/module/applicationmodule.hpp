#pragma once
#include <application/context/contexthelper.hpp>
#include <application/window/window.hpp>
#include <application/window/windowsystem.hpp>
#include <application/input/inputsystem.hpp>

namespace legion::application
{
    class ApplicationModule : public Module
    {
    public:
        virtual void setup() override
        {
            createProcessChain("Rendering");
            createProcessChain("Input");
            registerComponentType<window>();
            reportSystem<WindowSystem>();
            reportSystem<InputSystem>();
        }

        virtual priority_type priority() override
        {
            return 100;
        }

    };
}
