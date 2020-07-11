#pragma once
#include <core/engine/engine.hpp>

extern void reportModules(args::core::Engine* engine);

int main(int argc, char** argv)
{
	args::core::Engine engine = {};
	reportModules(&engine);

	return 0;
}