#pragma once
#include <core/types/types.hpp>
#include <core/ecs/ecsregistry.hpp>
#include <core/ecs/component_handle.hpp>

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

		component_type read(id_type entityId) { return component_handle<component_type>(entityId, registry).read(); }

		void write(id_type entityId, component_type&& value) { component_handle<component_type>(entityId, registry).write(value); }
		void fetch_add(id_type entityId, component_type&& value) { component_handle<component_type>(entityId, registry).fetch_add(value); }
		void fetch_multiply(id_type entityId, component_type&& value) { component_handle<component_type>(entityId, registry).fetch_multiply(value); }
		void destroy(id_type entityId) { component_handle<component_type>(entityId, registry).destroy(); }
	};
}