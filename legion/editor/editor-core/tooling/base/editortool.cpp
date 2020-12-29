#include <editor-core/tooling/base/editortool.hpp>

namespace legion::editor
{
    std::vector<std::string> EditorToolBase::getMenuOptions()
    {
        std::vector<std::string> menuNames;

        for (auto& [hash, ptr] : m_menuOptions)
        {
            menuNames.push_back(ptr->name);
        }

        return menuNames;
    }

    void EditorToolBase::AddMenuOption(const std::string& name, delegate<void()>&& func)
    {
        id_type id = nameHash(name);

        m_menuOptions.emplace(id, std::unique_ptr<menu_option_base>(new menu_option<delegate<void()>>(name, std::forward<delegate<void()>&&>(func))));
    }

}
