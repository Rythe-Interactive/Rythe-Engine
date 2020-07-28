#include <iostream>
#include <chrono>

#define ARGS_ENTRY
#include <core/core.hpp>

using namespace args::core;

#include "module/testModule.hpp"

struct sah
{
	int value;

	sah operator+(const sah& other)
	{
		return { value + other.value };
	}
};

void ARGS_CCONV reportModules(Engine* engine)
{
	std::cout << "Hello Args!" << std::endl;
	engine->reportModule<TestModule>();

	try
	{
		throw args_component_destroyed_error;
	}
	catch (const exception& e)
	{
		std::cout << e.what() << std::endl;
		std::cout << e.get_file() << std::endl;
		std::cout << e.get_line() << std::endl;
		std::cout << e.get_func() << std::endl;
	}

	std::cout << std::endl;

	ecs::EcsRegistry registry;

	std::cout << "registering component type" << std::endl;
	registry.reportComponentType<sah>();

	std::cout << "creating entity" << std::endl;

	ecs::entity_handle ent = registry.createEntity();

	if (ent)
		std::cout << "entity handle is valid" << std::endl;
	else
		std::cout << "entity handle is invalid" << std::endl;

	std::cout << "entity has id: " << ent.get_id() << std::endl;

	std::cout << "creating component through registry" << std::endl;
	registry.createComponent<sah>(ent);

	if (ent.has_component<sah>())
		std::cout << "entity has component" << std::endl;
	else
		std::cout << "entity does not have component" << std::endl;

	ecs::component_handle<sah> sahHandle = ent.get_component<sah>();

	if (sahHandle)
		std::cout << "component handle is valid" << std::endl;
	else
		std::cout << "component handle is invalid" << std::endl;

	std::cout << "component value is: " << sahHandle.read().value << std::endl;

	std::cout << "setting component value to 789" << std::endl;
	sahHandle.write({ 789 });
	std::cout << "component value is: " << sahHandle.read().value << std::endl;

	std::cout << "performing fetch_add 1" << std::endl;

	sahHandle.fetch_add({ 1 });
	std::cout << "component value is: " << sahHandle.read().value << std::endl;

	std::cout << "creating entity query through registry" << std::endl;
	ecs::EntityQuery query = registry.createQuery<sah>();

	std::cout << "query size is: " << query.size() << std::endl;

	std::cout << "iterating over query" << std::endl;
	bool found = false;
	for (ecs::entity_handle entity_handle : query)
	{
		if (entity_handle == ent)
		{
			found = true;
			std::cout << "our entity handle was found" << std::endl;
		}

		std::cout << "found entity with id: " << entity_handle.get_id() << std::endl;
	}

	if(!found)
		std::cout << "our entity handle was not found" << std::endl;

	std::cout << "destroying component" << std::endl;
	ent.remove_component<sah>();

	if (sahHandle)
		std::cout << "component handle is valid" << std::endl;
	else
		std::cout << "component handle is invalid" << std::endl;

	if (ent.has_component<sah>())
		std::cout << "entity has component" << std::endl;
	else
		std::cout << "entity does not have component" << std::endl;

	std::cout << "query size is: " << query.size() << std::endl;

	std::cout << "iterating over query" << std::endl;
	found = false;
	for (ecs::entity_handle entity_handle : query)
	{
		if (entity_handle == ent)
		{
			found = true;
			std::cout << "our entity handle was found" << std::endl;
		}

		std::cout << "found entity with id: " << entity_handle.get_id() << std::endl;
	}

	if (!found)
		std::cout << "our entity handle was not found" << std::endl;

	std::cout << "creating component through entity handle" << std::endl;
	ent.add_component<sah>();

	if (sahHandle)
		std::cout << "component handle is valid" << std::endl;
	else
		std::cout << "component handle is invalid" << std::endl;

	std::cout << "query size is: " << query.size() << std::endl;

	std::cout << "iterating over query" << std::endl;
	found = false;
	for (int i = 0; i < query.size(); i++)
	{
		if (query[i] == ent)
		{
			found = true;
			std::cout << "our entity handle was found" << std::endl;
		}

		std::cout << "found entity with id: " << query[i].get_id() << std::endl;
	}

	if (!found)
		std::cout << "our entity handle was not found" << std::endl;

	std::cout << "destroying entity" << std::endl;
	ent.destroy();

	if (ent)
		std::cout << "entity handle is valid" << std::endl;
	else
		std::cout << "entity handle is invalid" << std::endl;

	if (sahHandle)
		std::cout << "component handle is valid" << std::endl;
	else
		std::cout << "component handle is invalid" << std::endl;

	std::cout << "query size is: " << query.size() << std::endl;

	std::cout << "iterating over query" << std::endl;
	found = false;
	for (int i = 0; i < query.size(); i++)
	{
		if (query[i] == ent)
		{
			found = true;
			std::cout << "our entity handle was found" << std::endl;
		}

		std::cout << "found entity with id: " << query[i].get_id() << std::endl;
	}

	if (!found)
		std::cout << "our entity handle was not found" << std::endl;

	std::cout << std::endl;
}