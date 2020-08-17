#pragma once
#include <application/window/window.hpp>
#include <application/window/windowsystem.hpp>

namespace args::application
{
    class ApplicationModule : public Module
    {
    public:
        virtual void setup() override
        {
            reportComponentType<window>();
            reportSystem<WindowSystem>();
        }
    };
}
