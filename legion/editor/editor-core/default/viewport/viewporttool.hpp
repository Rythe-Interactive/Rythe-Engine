#pragma once
#include <editor-core/tooling/editortool.hpp>

namespace legion::editor
{
    class ViewportTool : public EditorTool<ViewportTool>
    {
        void setup();

        void openNewViewport();
    };
}
