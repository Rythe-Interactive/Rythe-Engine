#include <iostream>

#define ARGS_ENTRY
#define ARGS_KEEP_CONSOLE
#include <core/core.hpp>

#include <chrono>
#include <thread>

using namespace args;

#include "module/testmodule.hpp"
#include "systems/testsystem.hpp"

struct sah
{
	int value;

	sah operator+(const sah& other)
	{
		return { value + other.value };
	}

	sah operator*(const sah& other)
	{
		return { value * other.value };
	}
};

void ARGS_CCONV reportModules(Engine* engine)
{
	std::cout << "Hello Args!" << std::endl;
	engine->reportModule<TestModule>();
}