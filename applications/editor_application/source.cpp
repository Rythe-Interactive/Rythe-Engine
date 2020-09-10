#include <core/core.hpp>
#include <editor/editor.hpp>

void ARGS_CCONV reportEditorModules(args::editor::Editor* editor)
{

}

void ARGS_CCONV reportModules(args::core::Engine* engine)
{
	args::log::info("Hello Args!");
}
