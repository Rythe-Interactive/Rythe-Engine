#pragma once
#include <core/types/primitives.hpp>
#include <core/types/type_util.hpp>
#include <core/platform/platform.hpp>
#include <core/containers/sparse_map.hpp>

namespace args::core::ecs
{
	class ARGS_API QueryRegistry;
	class ARGS_API EcsRegistry;
	class ARGS_API entity_handle;

	class ARGS_API EntityQuery
	{
	private:
		QueryRegistry& m_registry;
		EcsRegistry& m_ecsRegistry;
		id_type m_id;

	public:
		EntityQuery(id_type id, QueryRegistry& registry, EcsRegistry& ecsRegistry) : m_registry(registry), m_ecsRegistry(ecsRegistry), m_id(id) {}

		sparse_map<id_type, entity_handle>::iterator begin();
		sparse_map<id_type, entity_handle>::const_iterator begin() const;
		sparse_map<id_type, entity_handle>::iterator end();
		sparse_map<id_type, entity_handle>::const_iterator end() const;

		template<typename component_type>
		void addComponentType() { addComponentType(typeHash<component_type>()); }

		void addComponentType(id_type componentTypeId);

		template<typename component_type>
		void removeComponentType() { removeComponentType(typeHash<component_type>()); }

		void removeComponentType(id_type componentTypeId);

		entity_handle operator[](size_type index);

		size_type size();
	};
}