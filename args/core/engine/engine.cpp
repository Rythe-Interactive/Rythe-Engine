#include <core/engine/engine.hpp>

/**
 * @file engine.cpp
 * @todo GlynLeine, this file needs to be documented
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