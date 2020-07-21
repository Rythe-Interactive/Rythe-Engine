#include <iostream>

#define ARGS_ENTRY
#include <core/core.hpp>

using namespace args::core;

#include "module/testModule.hpp"

struct sah
{
	std::string value;
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

	std::cout << "_____________________________________________________" << std::endl;


	sparse_map<int, sah> testMap;
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
	else
		std::cout << "map is not empty" << std::endl;

	std::cout << "testMap size is: " << testMap.size() << std::endl;


	for (auto val : testMap)
	{
		std::cout << val.first << "\t" << val.second.value << std::endl;
	}

	std::cout << "implicit add ijk and set to default value" << std::endl;
	std::cout << "i: " << testMap[i].value << std::endl;
	std::cout << "j: " << testMap[j].value << std::endl;
	std::cout << "k: " << testMap[k].value << std::endl;

	std::cout << std::endl;

	std::cout << "set k" << std::endl;
	testMap[k] = { "Nah" };

	for (auto val : testMap)
	{
		std::cout << val.first << "\t" << val.second.value << std::endl;
	}
	std::cout << "i: " << testMap[i].value << std::endl;
	std::cout << "j: " << testMap[j].value << std::endl;
	std::cout << "k: " << testMap[k].value << std::endl;

	std::cout << "testMap size is: " << testMap.size() << std::endl;
	std::cout << "testMap capacity is: " << testMap.capacity() << std::endl;

	std::cout << "_____________________________________________________" << std::endl << std::endl;

	sparse_set<uint> testSet;
	std::cout << "testSet size is: " << testSet.size() << std::endl;
	std::cout << "testSet capacity is: " << testSet.capacity() << std::endl;

	std::cout << "insert items to testSet: 43, 12, 30, 24" << std::endl;

	testSet.insert(43);
	testSet.insert(12);
	testSet.insert(30);
	testSet.insert(24);

	std::cout << "testSet size is: " << testSet.size() << std::endl;
	std::cout << "testSet capacity is: " << testSet.capacity() << std::endl;


	std::cout << "iterate using iterator" << std::endl;
	for (auto val : testSet)
	{
		std::cout << val << std::endl;
	}

	if (testSet.contains(12))
		std::cout << "testSet does contain 12" << std::endl;
	else
		std::cout << "testSet doesn't contain 12" << std::endl;

	std::cout << "erased 12" << std::endl;
	testSet.erase(12);

	std::cout << "iterate using index" << std::endl;
	for (int i = 0; i < testSet.size(); i++)
	{
		std::cout << testSet[i] << std::endl;
	}

	if (testSet.contains(12))
		std::cout << "testSet does contain 12" << std::endl;
	else
		std::cout << "testSet doesn't contain 12" << std::endl;

	std::cout << "cleared set" << std::endl;
	testSet.clear();

	if (testSet.empty())
		std::cout << "testSet is empty" << std::endl;
	else
		std::cout << "testSetis not empty" << std::endl;


	std::cout << "iterate using index" << std::endl;
	for (int i = 0; i < testSet.size(); i++)
	{
		std::cout << testSet[i] << std::endl;
	}

	if (testSet.contains(12))
		std::cout << "testSet does contain 12" << std::endl;
	else
		std::cout << "testSet doesn't contain 12" << std::endl;

	std::cout << "testSet size is: " << testSet.size() << std::endl;
	std::cout << "testSet capacity is: " << testSet.capacity() << std::endl;

	std::cout << "_____________________________________________________" << std::endl;


}