#pragma once
#include <core/engine/engine.hpp>
#include <iostream>

extern void reportModules(args::core::Engine* engine);

#ifdef ARGS_ENTRY
int main(int argc, char** argv)
{
	args::core::Engine engine;
	reportModules(&engine);

	engine.init();

#ifdef _DEBUG
	_getwch();
#endif // DEBUG
	return 0;
}
#endif