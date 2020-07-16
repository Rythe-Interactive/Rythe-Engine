#include <core/engine/engine.hpp>

/**
 * @file engine.cpp
 */

namespace args::core
{
	void Engine::init()
	{
		for (auto priority : modules)
			for (auto* module : priority.second)
			{
				module->init();
			}
	}
}