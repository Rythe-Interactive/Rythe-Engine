#include <future>
#include <core/engine/engine.hpp>

/**
 * @file engine.cpp
 */

namespace args::core
{
	void Engine::init()
	{
		for (const auto& priority : modules)
			for (auto* module : priority.second)
			{
				module->init();
			}
	}

	void Engine::run()
	{
		// needs implementation.
	}
}