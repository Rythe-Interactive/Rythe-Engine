#pragma once

/**
 * @file editormodule.hpp
 */

namespace args::editor
{
	/**@class EditorModule
	 * @brief interface for editor-modules, must be implemented
	 * @ref args::editor::Editor::reportModule<T,...>()
	 */
	class EditorModule
	{
	};

	struct editor_module_initializer_t {};
}

