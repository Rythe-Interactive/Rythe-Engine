#pragma once

/**
 * @file editormodule.hpp
 */

namespace legion::editor
{
	/**@class EditorModule
	 * @brief interface for editor-modules, must be implemented
	 * @ref legion::editor::Editor::reportModule<T,...>()
	 */
	class EditorModule
	{
	};

	struct editor_module_initializer_t {};
}

