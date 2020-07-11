#pragma once
#include <core/platform/platform.hpp>
#include <core/engine/module.hpp>
#include <core/types/primitives.hpp>
#include <core/types/sfinae.hpp>

#include <map>
#include <vector>

namespace args::core
{
	class ARGS_API Engine
	{
	private:
		std::map<priority_type, std::vector<Module*>> modules = {};

	public:
		Engine() {};

		template<class ModuleType, inherits_from<ModuleType, Module> = 0>
		void reportModule()
		{
			auto* module = new ModuleType();
			priority_type priority = module->priority();
			modules[priority].push_back(module);
		}

		void init();		
	};
}
