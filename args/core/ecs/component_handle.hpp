#pragma once
#include <atomic>
#include <core/ecs/ecsregistry.hpp>
#include <core/ecs/component_container.hpp>
#include <core/ecs/entity.hpp>
#include <core/common/common.hpp>

/**
 * @file component_handle.hpp
 */

namespace args::core::ecs
{
	/**@class component_handle
	 * @brief Handle to components that allow safe component loading and storing.
	 * @tparam component_type Type of targeted component.
	 */
	template<typename component_type>
	class component_handle
	{
	public:
		// Entity that owns this component.
		const entity entity;

	private:
		EcsRegistry* registry;

	public:
		component_handle(id_type entityId, EcsRegistry* registry) : registry(registry), entity(entityId, registry) {}

		/**@brief Atomic read of component.
		 * @param order Memory order at which to load the component.
		 * @returns component_type Current value of component.
		 */
		component_type read(std::memory_order order = std::memory_order_acquire)
		{
			std::atomic<component_type>* comp = registry->getFamily<component_type>()->get_component(entity);
			assert_msg("Component no longer exists.", comp);

			return comp->load(order);
		}

		/**@brief Atomic write of component.
		 * @param value Value you wish to write.
		 * @param order Memory order at which to write the component.
		 * @returns component_type Current value of component.
		 */
		component_type write(component_type&& value, std::memory_order order = std::memory_order_release)
		{
			std::atomic<component_type>* comp = registry->getFamily<component_type>()->get_component(entity);
			assert_msg("Component no longer exists.", comp);

			comp->store(value, order);

			return value;
		}

		/**@brief Atomic read modify write with add modification on component.
		 * @param value Value you wish to add.
		 * @param loadOrder Memory order at which to load the component.
		 * @param successOrder Memory order upon success of CAS-loop.
		 * @param failureOrder Memory order upon failure of CAS-loop.
		 * @returns component_type Current value of component.
		 */
		component_type fetch_add(component_type&& value,
			std::memory_order loadOrder = std::memory_order_acquire,
			std::memory_order successOrder = std::memory_order_release,
			std::memory_order failureOrder = std::memory_order_relaxed)
		{
			std::atomic<component_type>* comp = registry->getFamily<component_type>()->get_component(entity);
			assert_msg("Component no longer exists.", comp);

			component_type oldVal = comp->load(loadOrder);
			component_type newVal = oldVal + value;

			while (!comp->compare_exchange_strong(oldVal, newVal, successOrder, failureOrder))
				newVal = oldVal + value;

			return newVal;
		}

		/**@brief Atomic read modify write with multiply modification on component.
		 * @param value Value you wish to multiply by.
		 * @param loadOrder Memory order at which to load the component.
		 * @param successOrder Memory order upon success of CAS-loop.
		 * @param failureOrder Memory order upon failure of CAS-loop.
		 * @returns component_type Current value of component.
		 */
		component_type fetch_multiply(component_type&& value,
			std::memory_order loadOrder = std::memory_order_acquire,
			std::memory_order successOrder = std::memory_order_release,
			std::memory_order failureOrder = std::memory_order_relaxed)
		{
			std::atomic<component_type>* comp = registry->getFamily<component_type>()->get_component(entity);
			assert_msg("Component no longer exists.", comp);

			component_type oldVal = comp->load(loadOrder);
			component_type newVal = oldVal * value;

			while (!comp->compare_exchange_strong(oldVal, newVal, successOrder, failureOrder))
				newVal = oldVal * value;

			return newVal;
		}

		/**@brief Locks component family and destroys component.
		 */
		void destroy() { registry->getFamily<component_type>()->destroy_component(entity); }
	};
}