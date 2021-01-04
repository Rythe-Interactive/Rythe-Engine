#include <editor-core/tooling/editortool.hpp>

namespace legion::editor
{
    void EditorToolBase::addMenuOption(const std::string& name, delegate<void()>&& func, const std::string& tooltip)
    {
        MenuRenderer::addMenuOption(name, func, tooltip);
    }

    void EditorToolBase::enableMenuOption(const std::string& name, bool enabled)
    {
        MenuRenderer::enableMenuOption(name, enabled);
    }

    void EditorToolBase::setToolTip(const std::string& name, const std::string& tooltip)
    {
        MenuRenderer::setToolTip(name, tooltip);
    }

    void EditorToolBase::openSurface(std::unique_ptr<EditorSurfaceBase>&& surface)
    {
        SurfaceRenderer::openSurface(std::forward<std::unique_ptr<EditorSurfaceBase>&&>(surface));
    }

}
