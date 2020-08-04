#include <future>
#include <core/engine/engine.hpp>

/**
 * @file engine.cpp
 */

namespace args::core
{
	void Engine::init()
	{
		for (const auto& [priority, moduleList] : modules)
			for (auto& module : moduleList)
			{
				module->init();
			}
	}

	void Engine::run()
	{
		// needs implementation.
	}
}