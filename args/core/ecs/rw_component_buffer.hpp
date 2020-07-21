#pragma once
#include <core/common/exception.hpp>
#include <core/types/types.hpp>
#include <core/ecs/ecsregistry.hpp>
#include <core/ecs/entity.hpp>
#include <core/ecs/component_handle.hpp>
#include <core/ecs/component_container.hpp>

/**@todo documentation.
 */

namespace args::core::ecs
{
	template<typename component_type>
	class rw_component_buffer
	{
	private:
		EcsRegistry& m_registry;

	public:
		/**@brief Creates component buffer.
		 */
		rw_component_buffer(EcsRegistry& registry) : m_registry(registry) {}

		/// Makes more sense to implement with index_type in combination with entity_query
		//component_handle<component_type> operator[](id_type entityId)
		//{
		//}

		/**@brief Atomic read of component.
		 * @param entityId ID of entity you wish to fetch the component from.
		 * @param order Memory order at which to load the component.
		 * @returns component_type Current value of component.
		 */
		component_type read(id_type entityId, std::memory_order order = std::memory_order_acquire)
		{
			std::atomic<component_type>* comp = m_registry.getFamily<component_type>()->get_component(entityId);
			if (!comp)
				throw args_component_destroyed_error;

			return comp->load(order);
		}

		/**@brief Atomic write of component.
		 * @param entityId ID of entity you wish to overwrite the component of.
		 * @param value Value you wish to write.
		 * @param order Memory order at which to write the component.
		 * @returns component_type Current value of component.
		 */
		void write(id_type entityId, component_type&& value, std::memory_order order = std::memory_order_release)
		{
			std::atomic<component_type>* comp = m_registry.getFamily<component_type>()->get_component(entityId);
			if (!comp)
				throw args_component_destroyed_error;

			comp->store(value, order);

			return value;
		}

		/**@brief Atomic read modify write with add modification on component.
		 * @param entityId ID of entity you wish to add onto component of.
		 * @param value Value you wish to add.
		 * @param loadOrder Memory order at which to load the component.
		 * @param successOrder Memory order upon success of CAS-loop.
		 * @param failureOrder Memory order upon failure of CAS-loop.
		 * @returns component_type Current value of component.
		 */
		void fetch_add(id_type entityId, component_type&& value,
			std::memory_order loadOrder = std::memory_order_acquire,
			std::memory_order successOrder = std::memory_order_release,
			std::memory_order failureOrder = std::memory_order_relaxed)
		{
			std::atomic<component_type>* comp = m_registry.getFamily<component_type>()->get_component(entityId);
			if (!comp)
				throw args_component_destroyed_error;

			component_type oldVal = comp->load(loadOrder);
			component_type newVal = oldVal + value;

			while (!comp->compare_exchange_weak(oldVal, newVal, successOrder, failureOrder))
				newVal = oldVal + value;

			return newVal;
		}

		/**@brief Atomic read modify write with multiply modification on component.
		 * @param entityId ID of entity you wish to multiply to component of.
		 * @param value Value you wish to multiply by.
		 * @param loadOrder Memory order at which to load the component.
		 * @param successOrder Memory order upon success of CAS-loop.
		 * @param failureOrder Memory order upon failure of CAS-loop.
		 * @returns component_type Current value of component.
		 */
		void fetch_multiply(id_type entityId, component_type&& value,
			std::memory_order loadOrder = std::memory_order_acquire,
			std::memory_order successOrder = std::memory_order_release,
			std::memory_order failureOrder = std::memory_order_relaxed)
		{
			std::atomic<component_type>* comp = m_registry.getFamily<component_type>()->get_component(entityId);
			if (!comp)
				throw args_component_destroyed_error;

			component_type oldVal = comp->load(loadOrder);
			component_type newVal = oldVal * value;

			while (!comp->compare_exchange_weak(oldVal, newVal, successOrder, failureOrder))
				newVal = oldVal * value;

			return newVal;
		}

		/**@brief Locks component family and destroys component.
		 * @param entityId ID of entity you wish to remove the component from.
		 * @ref args::core::ecs::component_container::destroy_component
		 */
		void destroy(id_type entityId) { m_registry.getFamily<component_type>()->destroy_component(entityId); }
	};
}