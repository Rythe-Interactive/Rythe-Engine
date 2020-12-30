#include <editor-core/tooling/base/editorsurface.hpp>

namespace legion::editor
{
    EditorSurfaceBase::EditorSurfaceBase(const std::string& name) : m_name(name)
    {
        setup(m_name);
        m_id = nameHash(m_name);
    }

    id_type EditorSurfaceBase::getId()
    {
        return m_id;
    }

    void EditorSurfaceBase::setName(const std::string& name)
    {
        m_name = name;
        m_id = nameHash(name);
    }

    const std::string& EditorSurfaceBase::getName()
    {
        return m_name;
    }

    void EditorSurfaceBase::drawSurface(app::window& context, gfx::camera& cam, const gfx::camera::camera_input& camInput, time::span deltaTime)
    {
        imgui::base::Begin(m_name.c_str());
        onGUI(context, cam, camInput, deltaTime);
        imgui::base::End();
    }
}
