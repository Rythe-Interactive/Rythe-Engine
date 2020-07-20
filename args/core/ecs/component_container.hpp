#pragma once
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
		sparse_map<id_type, component_type> components;
	public:
		virtual bool has_component(id_type entityId) override { return components.contains(entityId); }

		component_type* get_component(id_type entityId)
		{
			if (components.contains(entityId))
				return &components[entityId];
			return nullptr;
		}

		component_type* create_component(id_type entityId) { return &components[entityId]; }

		virtual void destroy_component(id_type entityId) override { components.erase(entityId) }
	};
}