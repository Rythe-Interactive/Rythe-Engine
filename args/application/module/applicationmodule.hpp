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
            ContextHelper::init();
            window win;
            win.create(200, 200, "test");
            win.close();
            reportComponentType<window>();
            reportSystem<WindowSystem>();
        }
    };
}
