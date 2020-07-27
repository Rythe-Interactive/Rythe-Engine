#include <iostream>

#define ARGS_ENTRY
#include <core/core.hpp>


#include <core/common/result.hpp>
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
	catch (exception e)
	{
		std::cout << e.what() << std::endl;
		std::cout << e.get_file() << std::endl;
		std::cout << e.get_line() << std::endl;
		std::cout << e.get_func() << std::endl;
	}

	atomic_sparse_map<string, int> testMap;

	testMap["Hello"]->store(45, std::memory_order_relaxed);

	if (testMap.contains("Hello"))
		std::cout << "testMap contains \"Hello\" with value: " << testMap["Hello"]->load(std::memory_order_relaxed) << std::endl;
	else
		std::cout << "testMap does not contain \"Hello\"" << std::endl;

	try
	{
		testMap.erase("Hello");
	}
	catch (std::exception e)
	{
		std::cout << e.what() << std::endl;
	}

	if (testMap.contains("Hello"))
		std::cout << "testMap contains \"Hello\" with value: " << testMap["Hello"]->load(std::memory_order_relaxed) << std::endl;
	else
		std::cout << "testMap does not contain \"Hello\"" << std::endl;

	ecs::EcsRegistry registry;

	registry.reportComponentType<sah>();

	ecs::entity& ent = registry.createEntity();

	std::cout << "creating component" << std::endl;
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

	std::cout << "destroying component" << std::endl;
	registry.destroyComponent<sah>(ent);

	if (sahHandle)
		std::cout << "component handle is valid" << std::endl;
	else
		std::cout << "component handle is invalid" << std::endl;

	std::cout << "_____________________________________________________" << std::endl;

	using namespace args::core::common;
	
	result_decay<result<int,int>> decay(Ok(10));

	if(decay == valid_t{})
	{
		std::cout << "was valid!" << std::endl;
		std::cout << static_cast<int>(decay) << std::endl;
	}

	

	std::cout << "_____________________________________________________" << std::endl;
	if (ent.has_component<sah>())
		std::cout << "entity has component" << std::endl;
	else
		std::cout << "entity does not have component" << std::endl;
}