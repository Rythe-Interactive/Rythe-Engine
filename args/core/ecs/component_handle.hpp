#pragma once
#include <core/ecs/ecsregistry.hpp>
#include <core/ecs/entity.hpp>

namespace args::core::ecs
{
	template<typename component_type>
	class component_handle
	{
	public:
		const entity entity;

	private:
		EcsRegistry* registry;

	public:
		component_handle(id_type entityId, EcsRegistry* registry) : registry(registry), entity(entityId, registry) {}

		component_type read() { return *registry->getFamily<component_type>()->get_component(entity); }

		/**@todo make actually thread safe
		 * @todo make nullptr safe
		 */
		void write(component_type&& value) { *registry->getFamily<component_type>()->get_component(entity) = value; }
		void fetch_add(component_type&& value) { *registry->getFamily<component_type>()->get_component(entity) += value }
		void fetch_multiply(component_type&& value) { *registry->getFamily<component_type>()->get_component(entity) *= value }
		void destroy() { registry->getFamily<component_type>()->destroy_component(entity); }
	};
}