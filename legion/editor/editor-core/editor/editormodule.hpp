#pragma once
#include <core/core.hpp>

/**
 * @file editormodule.hpp
 */

namespace legion::editor
{
    namespace detail
    {
        class EditorEngineModule;
    }

    class EditorModuleBase : private Module
    {
    protected:
        template<typename ToolType, typename... Args, inherits_from<ToolType, System<ToolType>> = 0>
        void reportTool(Args&&... args)
        {
            reportSystem<ToolType>(std::forward<Args>(args)...);
        }
    };

    /**@class EditorModule
     * @brief interface for editor-modules, must be implemented
     * @ref legion::editor::Editor::reportModule<T,...>()
     */
    template<typename EngineModule>
    class EditorModule : public EditorModuleBase
    {
        friend class Editor;
        friend class legion::editor::detail::EditorEngineModule;
    private:
        EngineModule* m_engineModule;

        using EngineModuleType = EngineModule;

    protected:
        EngineModule* getEngineModule()
        {
            return m_engineModule;
        }

        template<typename... Args>
        void parameteriseEngineModule(Args&&... args)
        {
            m_engineModule = new EngineModule(std::forward<Args>(args)...);
        }
    };

    using PureEditorModule = EditorModule<void>;

    template<>
    class EditorModule<void> : public EditorModuleBase
    {
        friend class Editor;
        friend class legion::editor::detail::EditorEngineModule;
    private:
        using EngineModuleType = std::nullptr_t;
    };

    template<>
    class EditorModule<std::nullptr_t> : public EditorModuleBase
    {
        friend class Editor;
        friend class legion::editor::detail::EditorEngineModule;
    private:
        using EngineModuleType = std::nullptr_t;
    };
}
