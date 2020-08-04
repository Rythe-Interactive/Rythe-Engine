#pragma once
#include <core/platform/platform.hpp>
#include <core/engine/module.hpp>
#include <core/types/primitives.hpp>
#include <core/types/meta.hpp>

#include <map>
#include <vector>
#include <memory>

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
	class Engine
	{
	private:
		std::map<priority_type, std::vector<std::unique_ptr<Module>>, std::greater<>> modules;

	public:
		/**@brief reports an engine module
		 * @tparam ModuleType the module you want to report
		 * @note ModuleType must be default constructible
		 * @ref args::core::Module
		 */
		template<class ModuleType, inherits_from<ModuleType, Module> = 0>
		void reportModule()
		{
			std::unique_ptr<Module> module = std::make_unique<ModuleType>();
			const priority_type priority = module->priority();
			modules[priority].emplace_back(std::move(module));
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
			std::unique_ptr<Module> module = std::make_unique<ModuleType>(std::forward<Args>(args)...);
			const priority_type priority = module->priority();
			modules[priority].emplace_back(std::move(module));
		}

		/**@brief Calls init on all reported modules and thus engine internals.
		 * @note Needs to be called manually if ARGS_ENTRY was not used.
		 * @ref args::core::Module
		 */
		void init()
		{
			for (const auto& [priority, moduleList] : modules)
				for (auto& module : moduleList)
				{
					module->init();
				}
		}

		/**@brief Runs engine loop.
		 */
		void run()
		{
			// needs implementation.
		}
	};
}
