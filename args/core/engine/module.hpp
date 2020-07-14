#pragma once
#include <core/types/primitives.hpp>
#include <core/platform/platform.hpp>
/**
 * @file module.hpp
 * @todo GlynLeine, this file needs to be reviewed
 */

namespace args::core
{
	/**@class Module
	 * @brief interface for engine-modules, must be implemented 
	 */
	class Module
	{
	public:

		/**@brief determines the execution priority of this module
		 * @ref ARGS_PURE
		 * @returns priority_type signed int8 higher is higher priority
		 */
		virtual priority_type priority() ARGS_PURE;

		/**@brief module initializer
		 * @ref ARGS_PURE
		 */
		virtual void init() ARGS_PURE;
	};

	struct module_initializer_t{};
	
}
