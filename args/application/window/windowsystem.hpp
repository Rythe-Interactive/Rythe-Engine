#pragma once
#include <application/window/window.hpp>
#include <application/context/contexthelper.hpp>

/**@file windowsystem.hpp
*/

namespace args::application
{
    class WindowSystem final : public System<WindowSystem>
    {
    public:
        virtual void setup()
        {
        }
    };
}
