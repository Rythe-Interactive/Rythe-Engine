#pragma once
#include <editor-core/tooling/base/editorsurface.hpp>
#include <editor-core/tooling/base/editorgizmo.hpp>
#include <editor-core/data/menu_option.hpp>

namespace legion::editor
{
    class EditorToolBase
    {
    public:
        virtual void Setup() LEGION_PURE;

        std::vector<std::string> getMenuOptions();
    protected:
        template<typename FuncType>
        void AddMenuOption(const std::string& name, FuncType func)
        {
            id_type id = nameHash(name);

            m_menuOptions.emplace(id, std::unique_ptr<menu_option_base>(new menu_option<FuncType>(name, std::forward<FuncType>(func))));
        }

        void AddMenuOption(const std::string& name, delegate<void()>&& func);

        void OpenSurface(std::unique_ptr<EditorSurfaceBase>&& surface);

        template<typename SurfaceType, inherits_from<SurfaceType, EditorSurface<SurfaceType>> = 0>
        void OpenSurface()
        {

        }

    private:
        std::unordered_map<id_type, std::unique_ptr<menu_option_base>> m_menuOptions;
    };

    template<typename SelfType>
    class EditorTool : public EditorToolBase
    {
    protected:
        template<void(SelfType::* func_type)()>
        void AddMenuOption(const std::string& name)
        {
            id_type id = nameHash(name);

            m_menuOptions.emplace(id, std::unique_ptr<menu_option_base>(new menu_option<delegate<void()>>(name, delegate<void()>::create<SelfType, func_type>((SelfType*)this))));            
        }
    };
}
