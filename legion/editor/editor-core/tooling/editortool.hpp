#pragma once
#include <editor-core/tooling/editorsurface.hpp>
#include <editor-core/tooling/editorgizmo.hpp>
#include <editor-core/data/menu_option.hpp>
#include <editor-core/systems/surfacerenderer.hpp>
#include <editor-core/systems/menurenderer.hpp>

namespace legion::editor
{
    using menu_option_info = std::tuple<std::string, std::string&, bool>;

    class EditorToolBase
    {
    protected:
        template<typename FuncType>
        void addMenuOption(const std::string& name, FuncType func, const std::string& tooltip = "")
        {
            MenuRenderer::addMenuOption<FuncType>(name, func, tooltip);
        }
        void addMenuOption(const std::string& name, delegate<void()>&& func, const std::string& tooltip = "");

        void enableMenuOption(const std::string& name, bool enabled = true);
        void disableMenuOption(const std::string& name) { enableMenuOption(name, false); }

        void setToolTip(const std::string& name, const std::string& tooltip);

        void openSurface(std::unique_ptr<EditorSurfaceBase>&& surface);

        template<typename SurfaceType, typename... Args, inherits_from<SurfaceType, EditorSurface<SurfaceType>> = 0>
        void openSurface(Args&&... args)
        {
            SurfaceRenderer::openSurface<SurfaceType>(std::forward<Args>(args)...);
        }
    };

    template<typename SelfType>
    class EditorTool : public EditorToolBase, public System<SelfType>
    {
    protected:
        template<void(SelfType::* func_type)()>
        void addMenuOption(const std::string& name, const std::string& tooltip = "")
        {
            MenuRenderer::addMenuOption(name, delegate<void()>(static_cast<SelfType*>(this), func_type), tooltip);
        }
    };
}
