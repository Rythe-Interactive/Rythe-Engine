#include <iostream>

#define ARGS_ENTRY
#define ARGS_KEEP_CONSOLE

#include <chrono>
#include <thread>

#include <core/core.hpp>
#include "core/common/result.hpp"

#include <application/application.hpp>

#include "module/testModule.hpp"
#include "systems/testsystem.hpp"

#include "test_filesystem.hpp"

using namespace args;

void ARGS_CCONV reportModules(Engine* engine)
{
    app::window win;
    win.create(200, 200, "test window");
    win.makeCurrent();
    win.display();
    math::ivec2 size = win.getFramebufferSize();

    std::cout << size.x << ", " << size.y << std::endl;

	std::cout << "Hello Args!" << std::endl;

    test_filesystem();

	engine->reportModule<TestModule>();

	try
	{
		throw args_component_destroyed_error;
	}
	catch (const exception& e)
	{
		std::cout << e.what() << std::endl;
		std::cout << e.file() << std::endl;
		std::cout << e.line() << std::endl;
		std::cout << e.func() << std::endl;
	}    
}
