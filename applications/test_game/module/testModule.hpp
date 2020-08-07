#pragma once
#include <core/core.hpp>
#include "../systems/testsystem.hpp"

using namespace args;

class TestModule : public Module
{
public:
	virtual void setup() override
	{
		addChain("TestChain");

		reportComponentType<sah>();
		reportSystem<TestSystem>();
	}

	virtual priority_type priority() override
	{
		return default_priority;
	}
};

