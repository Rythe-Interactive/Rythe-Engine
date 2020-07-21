#pragma once
#include <core/async/readonly_rw_spinlock.hpp>
#include <core/platform/platform.hpp>
#include <core/containers/sparse_map.hpp>
#include <core/types/types.hpp>

/**
 * @file component_container.hpp
 */

namespace args::core::ecs
{
	/**@class component_container_base
	 * @brief Base class of args::core::ecs::component_container
	 */
	class component_container_base
	{
	public:
		virtual bool has_component(id_type entityId) ARGS_PURE;
		virtual void create_component(id_type entityId) ARGS_PURE;
		virtual void destroy_component(id_type entityId) ARGS_PURE;
	};

	/**@class component_container
	 * @brief Thread-safe container to store a component family in.
	 * @tparam component_type Type of component.
	 */
	template<typename component_type>
	class component_container : public component_container_base
	{
	private:
		sparse_map<id_type, std::atomic<component_type>> components;
		async::readonly_rw_spinlock lock;

	public:
		component_container() : components(), lock() {}

		/**@brief Checks whether entity has the component.
		 * @note Thread will be halted if there are any writes until they are finished.
		 * @note Will trigger read on this container.
		 * @param entityId ID of the entity you wish to check for.
		 * @ref args::core::async::readonly_rw_spinlock
		 */
		virtual bool has_component(id_type entityId) override
		{
			async::readonly_guard guard(lock);
			return components.contains(entityId);
		}

		/**@brief Fetches std::atomic wrapped component.
		 * @note Thread will be halted if there are any writes until they are finished.
		 * @note Will trigger read on this container.
		 * @param entityId ID of entity you want to get the component from.
		 * @returns std::atomic<component_type>* Pointer to std::atomic wrapped component.
		 * @ref args::core::async::readonly_rw_spinlock
		 */
		std::atomic<component_type>* get_component(id_type entityId)
		{
			async::readonly_guard guard(lock);

			if (components.contains(entityId))
				return &components.get(entityId);
			return nullptr;
		}

		/**@brief Creates new std::atomic wrapped component.
		 * @note Thread will be halted if there are any reads or writes until they are finished.
		 * @note Will trigger write on this container.
		 * @param entityId ID of entity you wish to add the component to.
		 * @ref args::core::async::readonly_rw_spinlock
		 */
		virtual void create_component(id_type entityId) override
		{
			async::readwrite_guard guard(lock);

			components.emplace(entityId);
		}

		/**@brief Destroys component atomically.
		 * @note Thread will be halted if there are any reads or writes until they are finished.
		 * @note Will trigger write on this container.
		 * @param entityId ID of entity you wish to remove the component from.
		 * @ref args::core::async::readonly_rw_spinlock
		 */
		virtual void destroy_component(id_type entityId) override
		{
			async::readwrite_guard guard(lock);

			components.erase(entityId);
		}
	};
}