#include <editor-core/default/viewport/viewporttool.hpp>
#include <editor-core/default/viewport/viewportsurface.hpp>

void legion::editor::ViewportTool::setup()
{
    openSurface<ViewportSurface>();
    addMenuOption<&ViewportTool::openNewViewport>("main/view/viewport", "Open new viewport");
}

void legion::editor::ViewportTool::openNewViewport()
{
    openSurface<ViewportSurface>();
}
