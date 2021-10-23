#pragma once
#include <rendering/rendering.hpp>
#include <editor-core/editor/editormodule.hpp>

/**
 * @file editor.hpp
 */

namespace legion::editor
{
    namespace detail
    {
        class EditorEngineModule;
    }

    /**@class Editor
     * @brief The editors main object that handles editor modules.
     * @ref legion::editor::EditorModule
     */
    class Editor
    {
        friend class legion::editor::detail::EditorEngineModule;
    private:
        static Engine* m_engine;

        std::vector<std::unique_ptr<EditorModuleBase>> m_modules;

    public:
        Editor() = default;
        explicit Editor(Engine* engine);

        static Engine* getEngine();

        /**@brief Reports an editor module
         * @tparam ModuleType the module you want to report
         * @param args the arguments you want to pass
         * @ref legion::editor::EditorModule
         */
        template <class ModuleType, class... Args, inherits_from<ModuleType, EditorModuleBase> = 0>
        void reportModule(Args&&...args)
        {
            ModuleType* module = new ModuleType(std::forward<Args>(args)...);

            if constexpr (!std::is_same_v<typename ModuleType::EngineModuleType, std::nullptr_t>)
            {
                if (!module->m_engineModule)
                    module->m_engineModule = new ModuleType::EngineModuleType();
                m_engine->reportModule(std::unique_ptr<Module>(module->m_engineModule));
            }
            m_modules.push_back(std::unique_ptr<EditorModuleBase>(module));
        }
    };
}

/**@brief Reports editor modules to the editor, must be implemented by you.
 * @param [in] editor The editor object as ptr *
 * @ref legion::core::Editor::reportModule<T,...>()
 */
extern void reportEditorModules(legion::editor::Editor* editor);

#if defined(LEGION_ENTRY) && !defined(DISABLE_EDITOR_ENTRY) && !defined(DOXY_EXCLUDE)
namespace legion::editor::detail
{
    Editor editor;
}

void LEGION_CCONV reportModules(legion::Engine* engine)
{
    using namespace legion::editor;
    detail::editor = Editor(engine);
    engine->reportModule<lgn::app::ApplicationModule>();
    engine->reportModule<lgn::gfx::RenderingModule>();
    reportEditorModules(&detail::editor);
}
#endif
