#pragma once
#include <core/core.hpp>
#include <editor-core/editor/editormodule.hpp>

/**
 * @file editor.hpp
 */

namespace legion::editor
{
	/**@class Editor
	 * @brief The editors main object that handles editor modules.
	 * @ref legion::editor::EditorModule
	 */
	class Editor
	{
    public:
		/**@brief Reports an editor module
		 * @tparam ModuleType the module you want to report
		 * @param args the arguments you want to pass
		 * @ref legion::editor::EditorModule
		 */
        template <class ModuleType, class... Args CNDOXY(inherits_from<ModuleType, EditorModule> = 0)>
		void reportModule(Args&&...args)
		{

		}
	};

    namespace detail
    {
        class EditorEngineModule : public Module
        {
        public:
            EditorEngineModule(Editor* editor);

            virtual void setup();
        };
    }
}

/**@brief Reports editor modules to the editor, must be implemented by you.
 * @param [in] editor The editor object as ptr *
 * @ref legion::core::Editor::reportModule<T,...>()
 */
extern void reportEditorModules(legion::editor::Editor* editor);

#if defined(LEGION_ENTRY)
namespace legion::editor::detail
{
    Editor editor;
}

void LEGION_CCONV reportModules(legion::Engine* engine)
{
    using namespace legion::editor::detail;

    reportEditorModules(&editor);
    engine->reportModule<EditorEngineModule>(&editor);
}
#endif
