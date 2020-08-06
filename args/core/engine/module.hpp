#pragma once
#include <core/types/primitives.hpp>
#include <core/platform/platform.hpp>
#include <core/engine/system.hpp>
#include <core/containers/sparse_map.hpp>
#include <core/ecs/ecsregistry.hpp>
#include <core/scheduling/scheduler.hpp>
#include <memory>

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
		friend class Engine;
	private:
		ecs::EcsRegistry* m_ecs;
		scheduling::Scheduler* m_scheduler;

		sparse_map<id_type, SystemBase*> m_systems;

		void init()
		{
			for (auto* system : m_systems)
				system->setup();
		};

	protected:
		template<size_type charc>
		void addChain(const char(&name)[charc])
		{
			m_scheduler->addChain<charc>(name);
		}

		template<typename SystemType, typename... Args, inherits_from<SystemType, System<SystemType>> = 0>
		void reportSystem(Args&&... args)
		{
			SystemBase* system =  new SystemType(std::forward<Args>(args)...);
			system->m_ecs = m_ecs;
			system->m_scheduler = m_scheduler;

			m_systems.insert(typeHash<SystemType>(), system);
		}

	public:
		virtual void setup() ARGS_PURE;

		/**@brief determines the execution priority of this module
		 * @ref ARGS_IMPURE_RETURN
		 * @returns priority_type signed int8 higher is higher priority and get called first
		 * @note default priority of the engine is 0.
		 * @note call order for modules with the same priority is undefined.
		 */
		virtual priority_type priority() ARGS_IMPURE_RETURN(default_priority);

		virtual ~Module()
		{
			for (auto* system : m_systems)
				delete system;
		}
	};
}
