#pragma once
#include <core/core.hpp>

namespace legion::editor
{
    class EditorWindowSystem : public System<EditorWindowSystem>
    {
        virtual void setup();
    };
}
