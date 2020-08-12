#include <iostream>
#include <core/core.hpp>
#include <editor/editor.hpp>

void ARGS_CCONV reportEditorModules(args::editor::Editor* editor)
{

}

void ARGS_CCONV reportModules(args::core::Engine* engine)
{
	std::cout << "Hello Args!" << std::endl;
}