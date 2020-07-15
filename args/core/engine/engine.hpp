#pragma once
#include <core/platform/platform.hpp>
#include <core/engine/module.hpp>
#include <core/types/primitives.hpp>
#include <core/types/meta.hpp>

#include <map>
#include <vector>

/**
 * @file engine.hpp
 * @todo GlynLeine, this file needs to be documented
 */

namespace args::core
{
	class ARGS_API Engine
	{
	private:
		std::map<priority_type, std::vector<Module*>> modules = {};

	public:
		Engine() {};

		/**@brief reports an engine module
		 * @tparam ModuleType the module you want to report
		 * @note ModuleType must be default constructable
		 */
		template<class ModuleType, inherits_from<ModuleType, Module> = 0>
		void reportModule()
		{
			Module* module = new ModuleType();
			const priority_type priority = module->priority();
			modules[priority].push_back(module);
		}
		
		/**@brief reports an engine module
		 * @tparam ModuleType the module you want to report
		 * @param s a signal that you want to pass arguments to the constructor of the Module
		 * @param args the arguments you want to pass
		 */
		template <class ModuleType,class... Args, inherits_from<ModuleType,ModuleType> = 0>
		void reportModule(module_initializer_t s,Args&&...args)
		{
			(void) s;
			Module * module = new ModuleType(std::forward<Args>(args)...);
			const priority_type priority = module->priority();
			modules[priority].push_back(module);
		}

		
		void init();		
	};
}
