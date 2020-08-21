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
            reportComponentType<window_request>();
            reportComponentType<window>();
            reportSystem<WindowSystem>();
        }
    };
}
