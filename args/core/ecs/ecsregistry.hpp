#pragma once
#include <unordered_map>
#include <memory>
#include <core/common/exception.hpp>
#include <core/types/types.hpp>
#include <core/common/common.hpp>
#include <core/ecs/component_container.hpp>

namespace args::core::ecs
{
	class entity;

	class component_handle_base;

	template<typename component_type>
	class component_handle;

	class ARGS_API EcsRegistry
	{
	private:
		static id_type lastEntityId;
		std::unordered_map<id_type, component_container_base*> families;
		std::unordered_map<id_type, entity*> entities;

	public:
		EcsRegistry();

		template<typename component_type>
		void reportComponentType()
		{
			if (!families[typeHash<component_type>()])
				families[typeHash<component_type>()] = new component_container<component_type>();
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

		entity& createEntity();
		void destroyEntity(id_type entityId);
		entity& getEntity(id_type entityId);
	};
}