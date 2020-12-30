#pragma once
#include <editor-core/tooling/base/editorsurface.hpp>

namespace legion::editor
{
    class ViewportSurface : public EditorSurface<ViewportSurface>
    {
        virtual void setup(std::string& name) override;
        virtual void onGUI(app::window& context, gfx::camera& cam, const gfx::camera::camera_input& camInput, time::span deltaTime) override;
    };
}
