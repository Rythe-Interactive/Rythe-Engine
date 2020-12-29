#include <editor-core/tooling/base/editorsurface.hpp>

namespace legion::editor
{
    id_type EditorSurface::m_unnamedSurfaces = 0;

    EditorSurface::EditorSurface() : m_name("unnamed surface " + std::to_string(m_unnamedSurfaces++))
    {
    }

    EditorSurface::EditorSurface(const std::string& name) : m_name(name)
    {
    }

    void EditorSurface::setName(const std::string& name)
    {
        m_name = name;
    }

    const std::string& EditorSurface::getName()
    {
        return m_name;
    }

    void EditorSurface::drawSurface(app::window& context, gfx::camera& cam, const gfx::camera::camera_input& camInput, time::span deltaTime)
    {
        imgui::base::Begin(m_name.c_str());
        onGUI(context, cam, camInput, deltaTime);
        imgui::base::End();
    }
}
