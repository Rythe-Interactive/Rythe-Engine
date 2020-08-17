#pragma once
#include <application/window/window.hpp>

/**@file windowsystem.hpp
*/

namespace args::application
{
    class WindowSystem final : public System<WindowSystem>
    {
        virtual void setup(){}
    };
}
