#pragma once
#include <core/types/primitives.hpp>
#include <core/platform/platform.hpp>
/**
 * @file module.hpp
 */

namespace args::core
{
	/**@class Module
	 * @brief interface for engine-modules, must be implemented 
	 * @ref args::core::Engine::reportModule<T,...>()
	 */
	class Module
	{
	public:

		/**@brief determines the execution priority of this module
		 * @ref ARGS_IMPURE_RETURN
		 * @returns priority_type signed int8 higher is higher priority and get called first
		 * @note default priority of the engine is 0.
		 * @note call order for modules with the same priority is undefined.
		 */
		virtual priority_type priority() ARGS_IMPURE_RETURN(0);

		/**@brief module initializer
		 * @ref ARGS_PURE
		 */
		virtual void init() ARGS_PURE;
	};

	struct module_initializer_t{};
	
}
