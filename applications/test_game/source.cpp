#include <iostream>

#define ARGS_ENTRY
#include <core/core.hpp>

#include "module/testModule.hpp"

struct sah
{
	std::string value;
};

void ARGS_CCONV reportModules(args::core::Engine* engine)
{
	std::cout << "Hello Args!" << std::endl;
	engine->reportModule<TestModule>();

	std::cout << "_____________________________________________________" << std::endl;


	args::core::sparse_map<int, sah> testMap;
	int i = 46546;
	int j = 3415687;
	int k = 2648;
	testMap[i] = { "Hi" };
	testMap[j] = { "Hello" };
	testMap[k] = { "Nah" };

	std::cout << "i: " << i << ",\t" << testMap[i].value << std::endl;
	std::cout << "j: " << j << ",\t" << testMap[j].value << std::endl;
	std::cout << "k: " << k << ",\t" << testMap[k].value << std::endl;
	std::cout << std::endl;


	for (auto val : testMap)
	{
		std::cout << val.first << "\t" << val.second.value << std::endl;
	}
	std::cout << "i: " << testMap[i].value << std::endl;
	std::cout << "j: " << testMap[j].value << std::endl;
	std::cout << "k: " << testMap[k].value << std::endl;
	std::cout << std::endl;

	std::cout << "erase j" << std::endl;
	testMap.erase(j);

	for (auto val : testMap)
	{
		std::cout << val.first << "\t" << val.second.value << std::endl;
	}
	std::cout << "i: " << testMap[i].value << std::endl;
	std::cout << "j: " << testMap[j].value << std::endl;
	std::cout << "k: " << testMap[k].value << std::endl;

	std::cout << std::endl;

	std::cout << "clear" << std::endl;
	testMap.clear();

	if (testMap.empty())
		std::cout << "map is now empty" << std::endl;

	for (auto val : testMap)
	{
		std::cout << val.first << "\t" << val.second.value << std::endl;
	}
	std::cout << "i: " << testMap[i].value << std::endl;
	std::cout << "j: " << testMap[j].value << std::endl;
	std::cout << "k: " << testMap[k].value << std::endl;

	std::cout << std::endl;

	std::cout << "add k" << std::endl;
	testMap[k] = { "Nah" };

	for (auto val : testMap)
	{
		std::cout << val.first << "\t" << val.second.value << std::endl;
	}
	std::cout << "i: " << testMap[i].value << std::endl;
	std::cout << "j: " << testMap[j].value << std::endl;
	std::cout << "k: " << testMap[k].value << std::endl;

	std::cout << "_____________________________________________________" << std::endl;
}