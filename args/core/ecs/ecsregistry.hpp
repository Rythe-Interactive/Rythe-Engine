#pragma once
#include <core/containers/sparse_map.hpp>
#include <core/containers/sparse_set.hpp>
#include <memory>
#include <core/common/exception.hpp>
#include <core/types/types.hpp>
#include <core/common/common.hpp>
#include <core/ecs/component_container.hpp>
#include <core/ecs/queryregistry.hpp>

/**
 * @file ecsregistry.hpp
 */

namespace args::core::ecs
{
	class entity_handle;

	class component_handle_base;

	template<typename component_type>
	class component_handle;

	struct entity_data
	{
		id_type parent;
		sparse_map<id_type, entity_handle> children;
		sparse_map<id_type, id_type> components;
	};

	class ARGS_API EcsRegistry
	{
	private:
		static id_type m_lastEntityId;
		sparse_map<id_type, component_container_base*> m_families;
		sparse_map<id_type, entity_data> m_entityData;
		sparse_map<id_type, entity_handle> m_entities;
		QueryRegistry m_queryRegistry;

		void recursiveDestroyEntityInternal(id_type entityId);

	public:
		EcsRegistry();

		template<typename component_type>
		void reportComponentType()
		{
			if (!m_families.contains(typeHash<component_type>()))
				m_families[typeHash<component_type>()] = new component_container<component_type>();
		}

		template<typename component_type>
		component_container<component_type>* getFamily()
		{
			return static_cast<component_container<component_type>*>(getFamily(typeHash<component_type>()));
		}

		component_container_base* getFamily(id_type componentTypeId);

		template<typename component_type>
		component_handle<component_type> getComponent(id_type entityId)
		{
			return force_value_cast<component_handle<component_type>>(getComponent(entityId, typeHash<component_type>()));
		}

		component_handle_base getComponent(id_type entityId, id_type componentTypeId);

		template<typename component_type>
		component_handle<component_type> createComponent(id_type entityId)
		{
			return force_value_cast<component_handle<component_type>>(createComponent(entityId, typeHash<component_type>()));
		}

		component_handle_base createComponent(id_type entityId, id_type componentTypeId);

		template<typename component_type>
		void destroyComponent(id_type entityId)
		{
			destroyComponent(entityId, typeHash<component_type>());
		}

		void destroyComponent(id_type entityId, id_type componentTypeId);

		A_NODISCARD bool validateEntity(id_type entityId);

		entity_handle createEntity();

		void destroyEntity(id_type entityId, bool recurse = true);

		A_NODISCARD entity_handle getEntity(id_type entityId);

		A_NODISCARD entity_data& getEntityData(id_type entityId);

		A_NODISCARD sparse_map<id_type, entity_handle>& getEntities();

		template<typename... component_types>
		A_NODISCARD EntityQuery createQuery()
		{
			return m_queryRegistry.createQuery<component_types...>();
		}
	};
}