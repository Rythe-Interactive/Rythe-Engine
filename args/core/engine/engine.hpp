#pragma once
#include <core/platform/platform.hpp>
#include <core/engine/module.hpp>
#include <core/types/primitives.hpp>
#include <core/types/meta.hpp>

#include <map>
#include <vector>

/**
 * @file engine.hpp
 */

namespace args::core
{
	/**@class Engine
	 * @brief Main top level engine abstraction.
	 *        This class allows you to setup the engine with all the necessary modules and settings.
	 *        This class also contains all the sub managers of the core systems.
	 * @ref args::core::Module
	 * @ref args::core::ecs::EcsRegistry
	 * @ref args::core::scenes::SceneManager
	 * @ref args::core::scheduling::Scheduler
	 */
	class ARGS_API Engine
	{
	private:
		std::map<priority_type, std::vector<Module*>, std::greater<priority_type>> modules = {};

	public:
		Engine() {};

		/**@brief reports an engine module
		 * @tparam ModuleType the module you want to report
		 * @note ModuleType must be default constructable
		 * @ref args::core::Module
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
		 * @ref args::core::Module
		 */
		template <class ModuleType, class... Args, inherits_from<ModuleType, Module> = 0>
		void reportModule(module_initializer_t s, Args&&...args)
		{
			(void) s;
			Module * module = new ModuleType(std::forward<Args>(args)...);
			const priority_type priority = module->priority();
			modules[priority].push_back(module);
		}

		/**@brief Calls init on all reported modules and thus engine internals.
		 * @note Needs to be called manually if ARGS_ENTRY was not used.
		 * @ref args::core::Module
		 */
		void init();
	};
}
