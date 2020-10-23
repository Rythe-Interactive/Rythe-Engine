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
            addProcessChain("Rendering");
            addProcessChain("Input");
            reportComponentType<window>();
            reportSystem<WindowSystem>();
            reportSystem<InputSystem>();
        }

        virtual priority_type priority() override
        {
            return 100;
        }

    };
}
