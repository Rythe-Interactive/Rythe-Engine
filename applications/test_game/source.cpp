#include <iostream>

#define ARGS_ENTRY
#include <core/core.hpp>

using namespace args::core;

#include "module/testModule.hpp"

struct sah
{
	int value;
};

void ARGS_CCONV reportModules(Engine* engine)
{
	std::cout << "Hello Args!" << std::endl;
	engine->reportModule<TestModule>();

	try
	{
		throw args_component_destroyed_error;
	}
	catch (exception e)
	{
		std::cout << e.what() << std::endl;
		std::cout << e.get_file() << std::endl;
		std::cout << e.get_line() << std::endl;
		std::cout << e.get_func() << std::endl;
	}

	ecs::EcsRegistry registry;

	registry.reportComponentType<sah>();

	ecs::entity& ent = registry.createEntity();

	registry.createComponent<sah>(ent);

	if (ent.has_component<sah>())
		std::cout << "entity has component" << std::endl;
	else
		std::cout << "entity does not have component" << std::endl;

	registry.destroyComponent<sah>(ent);

	if (ent.has_component<sah>())
		std::cout << "entity has component" << std::endl;
	else
		std::cout << "entity does not have component" << std::endl;
}