#pragma once
#include <core/core.hpp>
#include "../systems/testsystem.hpp"

#include <core/math/math.hpp>

using namespace args;

class TestModule : public Module
{
public:
	virtual void setup() override
	{
		addProcessChain("TestChain");

		reportComponentType<sah>();
		reportSystem<TestSystem>();
	}

	virtual priority_type priority() override
	{
		return default_priority;
	}
};

