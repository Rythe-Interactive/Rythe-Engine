#pragma once
#include <core/core.hpp>
#include <rendering/rendering.hpp>

namespace legion::editor
{
    class EditorSurfaceBase
    {
    protected:
        std::string m_name;
        id_type m_id;

    public:
        EditorSurfaceBase() = default;
        explicit EditorSurfaceBase(const std::string& name);

        virtual void setup(const std::string& name) LEGION_PURE;

        id_type getId();
        virtual id_type getTypeId() LEGION_PURE;

        void setName(const std::string& name);
        const std::string& getName();

        void drawSurface(app::window& context, gfx::camera& cam, const gfx::camera::camera_input& camInput, time::span deltaTime);
        virtual void onGUI(app::window& context, gfx::camera& cam, const gfx::camera::camera_input& camInput, time::span deltaTime) LEGION_PURE;
    };

    template<typename SelfType>
    class EditorSurface : public EditorSurfaceBase
    {
    public:
        static const id_type surfaceType;
        EditorSurface() : EditorSurfaceBase("unnamed surface " + std::to_string(surfaceType)) {}
        explicit EditorSurface(const std::string& name) : EditorSurfaceBase(name) {}

        virtual id_type getTypeId()
        {
            return m_surfaceType;
        }
    };

    template<typename SelfType>
    const id_type EditorSurface<SelfType>::surfaceType = typeHash<SelfType>();
}
