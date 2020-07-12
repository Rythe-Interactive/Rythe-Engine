#include <iostream>
#include <core/core.hpp>
#include <editor/editor.hpp>

void __cdecl reportEditorModules(args::editor::Editor* editor)
{

}

void __cdecl reportModules(args::core::Engine* engine)
{
	std::cout << "Hello Args!" << std::endl;
	_getwch();
}