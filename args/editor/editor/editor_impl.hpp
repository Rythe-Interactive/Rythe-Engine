#pragma once
#include <core/platform/platform.hpp>
#include <core/types/types.hpp>
#include <editor/editor/editormodule.hpp>

/**
 * @file editor_impl.hpp
 */

namespace args::editor
{
	/**@class Editor
	 * @brief The editors main object that handles editor modules.
	 * @ref args::editor::EditorModule
	 */
	class ARGS_API Editor
	{
		/**@brief Reports an editor module.
		 * @tparam ModuleType The module you want to report.
		 * @note ModuleType must be default constructable.
		 * @ref args::editor::EditorModule
		 */
		template<class ModuleType, core::inherits_from<ModuleType, EditorModule> = 0>
		void reportModule()
		{
		}

		/**@brief Reports an editor module
		 * @tparam ModuleType the module you want to report
		 * @param s a signal that you want to pass arguments to the constructor of the Module
		 * @param args the arguments you want to pass
		 * @ref args::editor::EditorModule
		 */
		template <class ModuleType, class... Args, core::inherits_from<ModuleType, EditorModule> = 0>
		void reportModule(editor_module_initializer_t s, Args&&...args)
		{
		}
	};
}

/**@brief Reports editor modules to the editor, must be implemented by you.
 * @param [in] editor The editor object as ptr *
 * @ref args::core::Editor::reportModule<T,...>()
 */
extern void reportEditorModules(args::editor::Editor* editor);
