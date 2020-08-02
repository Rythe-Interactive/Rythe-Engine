#include <iostream>
#define ARGS_ENTRY
#include <core/core.hpp>

#include "module/testModule.hpp"

void ARGS_CCONV reportModules(args::core::Engine* engine)
{
	std::cout << "Hello Args!" << std::endl;
	engine->reportModule<TestModule>();
}