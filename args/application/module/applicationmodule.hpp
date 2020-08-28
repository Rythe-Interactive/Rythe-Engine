#pragma once
#include <application/context/contexthelper.hpp>
#include <application/window/window.hpp>
#include <application/window/windowsystem.hpp>

namespace args::application
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
        }

        virtual priority_type priority() override
        {
            return 100;
        }

    };
}
