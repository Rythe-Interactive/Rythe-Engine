#pragma once
#include <atomic>
#include <mutex>
#include <core/platform/platform.hpp>
#include <core/containers/sparse_map.hpp>
#include <core/types/types.hpp>

namespace args::core::ecs
{
	class component_container_base
	{
	public:
		virtual bool has_component(id_type entityId) ARGS_PURE;
		virtual void destroy_component(id_type entityId) ARGS_PURE;
	};

	template<typename component_type>
	class component_container : public component_container_base
	{
	private:
		enum read_state { idle = 0, read = 1, write = 2 };

		sparse_map<id_type, std::atomic<component_type>> components;
		std::atomic_int reading;
		std::atomic_int readers;
	public:
		/**@brief Checks whether entity has the component.
		 * @note Thread will be halted if a component is simultaneously destroyed until the destroy has finished.
		 * @param entityId ID of the entity you wish to check for.
		 */
		virtual bool has_component(id_type entityId) override
		{
			int state = read_state::idle;
			readers.fetch_add(1, std::memory_order_relaxed);
			while (!reading.compare_exchange_weak(state, read_state::read, std::memory_order_acquire, std::memory_order_relaxed))
			{
				if (state == read_state::write)
					state = read_state::idle;
				else
					break;
			}

			bool result = components.contains(entityId);
			readers.fetch_sub(1, std::memory_order_relaxed);

			if (readers.load(std::memory_order_acquire) == 0)
				reading.store(read_state::idle, std::memory_order_release);

			return result;
		}

		/**@brief Fetches std::atomic wrapped component.
		 */
		std::atomic<component_type>* get_component(id_type entityId)
		{
			if (components.contains(entityId))
				return &components[entityId];
			return nullptr;
		}

		/**@brief Creates new std::atomic wrapped component.
		 */
		std::atomic<component_type>* create_component(id_type entityId) { return &components[entityId]; }

		/**@brief Destroys component atomically.
		 * @note Thread will be halted if one or more 'has' checks are being executed or if another component is being destroyed untill all reads and writes are done.
		 * @param entityId ID of entity you wish to remove the component from.
		 */
		virtual void destroy_component(id_type entityId) override
		{
			int state = read_state::idle;
			while (!reading.compare_exchange_weak(state, read_state::write, std::memory_order_acquire, std::memory_order_relaxed))
				state = read_state::idle;

			components.erase(entityId);

			reading.store(read_state::idle, std::memory_order_release);
		}
	};
}