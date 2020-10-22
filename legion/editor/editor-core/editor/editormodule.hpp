#pragma once
#include <core/core.hpp>

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
        friend class Editor;
    public:
        virtual void setup() LEGION_PURE;
	};

    class CoreEditorModule : public EditorModule
    {
    public:
        virtual void setup();
    };
}
