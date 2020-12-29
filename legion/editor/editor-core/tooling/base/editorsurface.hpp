#pragma once
#include <core/core.hpp>
#include <rendering/rendering.hpp>

namespace legion::editor
{
    class EditorSurface
    {
    private:
        static id_type m_unnamedSurfaces;
        std::string m_name;

    public:
        EditorSurface();
        explicit EditorSurface(const std::string& name);

        void setName(const std::string& name);
        const std::string& getName();

        void drawSurface(app::window& context, gfx::camera& cam, const gfx::camera::camera_input& camInput, time::span deltaTime);
        virtual void onGUI(app::window& context, gfx::camera& cam, const gfx::camera::camera_input& camInput, time::span deltaTime) LEGION_PURE;
    };
}
