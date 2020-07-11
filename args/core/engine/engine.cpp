#include <core/engine/engine.hpp>

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