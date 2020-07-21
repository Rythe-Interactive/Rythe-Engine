#pragma once
#include <core/common/exception.hpp>
#include <core/types/types.hpp>
#include <core/ecs/ecsregistry.hpp>
#include <core/ecs/component_handle.hpp>

/**@todo documentation.
 */

namespace args::core::ecs
{
	template<typename component_type>
	class rw_component_buffer
	{
	private:
		EcsRegistry* registry;

	public:
		rw_component_buffer(EcsRegistry* registry) : registry(registry) {}

		component_handle<component_type> operator[](id_type entityId)
		{
			return component_handle<component_type>(entityId, registry);
		}

		component_type read(id_type entityId, std::memory_order order = std::memory_order_acquire)
		{
			std::atomic<component_type>* comp = registry->getFamily<component_type>()->get_component(entityId);
			if (!comp)
				throw args_component_destroyed_error;

			return comp->load(order);
		}

		void write(id_type entityId, component_type&& value, std::memory_order order = std::memory_order_release)
		{
			std::atomic<component_type>* comp = registry->getFamily<component_type>()->get_component(entityId);
			if (!comp)
				throw args_component_destroyed_error;

			comp->store(value, order);

			return value;
		}

		void fetch_add(id_type entityId, component_type&& value,
			std::memory_order loadOrder = std::memory_order_acquire,
			std::memory_order successOrder = std::memory_order_release,
			std::memory_order failureOrder = std::memory_order_relaxed)
		{
			std::atomic<component_type>* comp = registry->getFamily<component_type>()->get_component(entityId);
			if (!comp)
				throw args_component_destroyed_error;

			component_type oldVal = comp->load(loadOrder);
			component_type newVal = oldVal + value;

			while (!comp->compare_exchange_weak(oldVal, newVal, successOrder, failureOrder))
				newVal = oldVal + value;

			return newVal;
		}

		void fetch_multiply(id_type entityId, component_type&& value,
			std::memory_order loadOrder = std::memory_order_acquire,
			std::memory_order successOrder = std::memory_order_release,
			std::memory_order failureOrder = std::memory_order_relaxed)
		{
			std::atomic<component_type>* comp = registry->getFamily<component_type>()->get_component(entityId);
			if (!comp)
				throw args_component_destroyed_error;

			component_type oldVal = comp->load(loadOrder);
			component_type newVal = oldVal * value;

			while (!comp->compare_exchange_weak(oldVal, newVal, successOrder, failureOrder))
				newVal = oldVal * value;

			return newVal;
		}

		void destroy(id_type entityId) { registry->getFamily<component_type>()->destroy_component(entityId); }
	};
}