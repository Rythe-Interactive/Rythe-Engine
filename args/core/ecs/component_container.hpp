#pragma once
#include <atomic>
#include <mutex>
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
		enum read_state { idle = 0, read = 1, write = 2 };

		sparse_map<id_type, std::atomic<component_type>> components;
		std::atomic_int readState;
		std::atomic_int readers;

	public:
		/**@brief Checks whether entity has the component.
		 * @note Thread will be halted if there are any writes until they are finished.
		 * @note Will trigger read on this container.
		 * @param entityId ID of the entity you wish to check for.
		 */
		virtual bool has_component(id_type entityId) override
		{
			int state = read_state::idle;
			readers.fetch_add(1, std::memory_order_relaxed);
			while (!readState.compare_exchange_weak(state, read_state::read, std::memory_order_acquire, std::memory_order_relaxed))
			{
				if (state == read_state::write)
					state = read_state::idle;
				else
					break;
			}

			bool result = components.contains(entityId);
			readers.fetch_sub(1, std::memory_order_relaxed);

			if (readers.load(std::memory_order_acquire) == 0)
				readState.store(read_state::idle, std::memory_order_release);

			return result;
		}

		/**@brief Fetches std::atomic wrapped component.
		 * @note Thread will be halted if there are any writes until they are finished.
		 * @note Will trigger read on this container.
		 * @param entityId ID of entity you want to get the component from.
		 */
		std::atomic<component_type>* get_component(id_type entityId)
		{
			int state = read_state::idle;
			readers.fetch_add(1, std::memory_order_relaxed);
			while (!readState.compare_exchange_weak(state, read_state::read, std::memory_order_acquire, std::memory_order_relaxed))
			{
				if (state == read_state::write)
					state = read_state::idle;
				else
					break;
			}

			std::atomic<component_type>* result = nullptr;

			if (components.contains(entityId))
				result = &components.get(entityId);

			readers.fetch_sub(1, std::memory_order_relaxed);

			if (readers.load(std::memory_order_acquire) == 0)
				readState.store(read_state::idle, std::memory_order_release);

			return result;
		}

		/**@brief Creates new std::atomic wrapped component.
		 * @note Thread will be halted if there are any reads or writes until they are finished.
		 * @note Will trigger write on this container.
		 * @param entityId ID of entity you wish to add the component to.
		 */
		std::atomic<component_type>* create_component(id_type entityId)
		{
			int state = read_state::idle;
			while (!readState.compare_exchange_weak(state, read_state::write, std::memory_order_acquire, std::memory_order_relaxed))
				state = read_state::idle;

			std::atomic<component_type>* result = nullptr;
			auto emp = components.emplace(entityId);
			if (emp.second)
				result = &(emp.first->second);
			else
				result = &components.get(entityId);

			readState.store(read_state::idle, std::memory_order_release);

			return &components[entityId]; 
		}

		/**@brief Destroys component atomically.
		 * @note Thread will be halted if there are any reads or writes until they are finished.
		 * @note Will trigger write on this container.
		 * @param entityId ID of entity you wish to remove the component from.
		 */
		virtual void destroy_component(id_type entityId) override
		{
			int state = read_state::idle;
			while (!readState.compare_exchange_weak(state, read_state::write, std::memory_order_acquire, std::memory_order_relaxed))
				state = read_state::idle;

			components.erase(entityId);

			readState.store(read_state::idle, std::memory_order_release);
		}
	};
}