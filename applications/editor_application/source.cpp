#include <core/core.hpp>
#include <editor/editor.hpp>

void LEGION_CCONV reportEditorModules(legion::editor::Editor* editor)
{

}

void LEGION_CCONV reportModules(legion::core::Engine* engine)
{
	legion::log::info("Hello Legion!");
}
