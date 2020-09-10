#pragma once
#include <core/containers/containers.hpp>
#include <core/types/types.hpp>
#include <core/common/common.hpp>
#include <core/async/async.hpp>
#include <core/ecs/component_container.hpp>
#include <core/ecs/queryregistry.hpp>
#include <core/ecs/entityquery.hpp>
#include <core/ecs/entity_handle.hpp>
#include <core/ecs/archetype.hpp>

#include <utility>
#include <memory>

/**
 * @file ecsregistry.hpp
 */

namespace args::core::ecs
{
#define world_entity_id 1

	class component_handle_base;

	template<typename component_type>
	class component_handle;

	/**@class entity_data
	 * @brief Internal data-structure used to store hierarchy and composition data of an entity.
	 */
	struct entity_data
	{
		id_type parent;
        entity_set children;
		hashed_sparse_set<id_type> components;
	};

	/**@class EcsRegistry
	 * @brief Manager and owner of all ECS related objects.
	 */
	class ARGS_API EcsRegistry
	{
	private:
		static id_type m_nextEntityId;

		mutable async::readonly_rw_spinlock m_familyLock;
		sparse_map<id_type, std::unique_ptr<component_container_base>> m_families;

		mutable async::readonly_rw_spinlock m_entityDataLock;
		sparse_map<id_type, entity_data> m_entityData;

		mutable async::readonly_rw_spinlock m_entityLock;
        sparse_set<id_type> m_containedEntities;
		entity_set m_entities;

		QueryRegistry m_queryRegistry;
        events::EventBus* m_eventBus;

		/**@brief Internal function for recursively destroying all children and children of children etc.
		 */
		void recursiveDestroyEntityInternal(id_type entityId);

	public:
        entity_handle world;

		/**@brief Constructor initializes everything for the ECS and creates world entity.
		 */
		EcsRegistry(events::EventBus* eventBus);

		/**@brief Reports component type to the registry so that it can be stored managed and recognized as a component.
		 * @tparam component_type Type of struct you with to add as a component.
		 * @note For a struct to fully work as a component to all supported features of this ECS the struct needs the following requirements:
		 *		 - It needs to be default constructible.
		 *		 - It needs to be trivially copyable.
		 *		 - The + operator needs to be defined.
		 *		 - The * operator needs to be defined.
		 */
		template<typename component_type>
		void reportComponentType()
		{
			async::readwrite_guard guard(m_familyLock);
			if (!m_families.contains(typeHash<component_type>()))
				m_families[typeHash<component_type>()] = std::make_unique<component_container<component_type>>(this, m_eventBus);
		}

		/**@brief Get component storage of a certain type.
		 * @tparam component_type Type of the component you wish to fetch.
		 * @returns component_container<component_type>* Pointer to the component container that contains all components of the requested type.
		 * @throws args_unknown_component_error When component type is unknown.
		 */
		template<typename component_type>
		component_container<component_type>* getFamily()
		{
			return static_cast<component_container<component_type>*>(getFamily(typeHash<component_type>()));
		}

		/**@brief Get component storage of a certain type.
		 * @param componentTypeId Type id of the component you wish to fetch.
		 * @returns component_container_base* Pointer to the component container that contains all components of the requested type. (needs to be cast to original type to use)
		 * @throws args_unknown_component_error When component type is unknown.
		 */
		component_container_base* getFamily(id_type componentTypeId);

		/**@brief Get component handle of a certain type attached to a certain entity.
		 * @tparam component_type Type of component to get handle of.
		 * @param entityId Id of entity to get the component from.
		 * @returns component_handle<component_type> Component handle to the requested component. (may be invalid if the entity does not have this component type)
		 * @throws args_entity_not_found_error If the entity id does not belong to a valid entity.
		 */
		template<typename component_type>
		component_handle<component_type> getComponent(id_type entityId)
		{
			return force_value_cast<component_handle<component_type>>(getComponent(entityId, typeHash<component_type>()));
		}

		/**@brief Get component handle of a certain type attached to a certain entity.
		 * @param entityId Id of entity to get the component from.
		 * @param componentTypeId Type id of component to get handle of.
		 * @returns component_handle_base Component handle to the requested component. (may be invalid if the entity does not have this component type)
		 * @note Handle needs to force_cast to component_handle<T> in order to use correctly.
		 * @throws args_entity_not_found_error If the entity id does not belong to a valid entity.
		 */
		component_handle_base getComponent(id_type entityId, id_type componentTypeId);

		/**@brief Create component of a certain type attached to a certain entity.
		 * @tparam component_type Type of component you wish to create.
		 * @param entityId Id of the entity you wish to attach the component to.
		 * @returns component_handle<component_type> Component handle to the created component.
		 * @throws args_entity_not_found_error If the entity id does not belong to a valid entity.
		 */
		template<typename component_type, typename = doesnt_inherit_from<component_type, archetype_base>>
		component_handle<component_type> createComponent(id_type entityId)
		{
            return force_value_cast<component_handle<component_type>>(createComponent(entityId, typeHash<component_type>()));
		}

        template<typename component_type, typename = doesnt_inherit_from<component_type, archetype_base>>
        component_handle<component_type> createComponent(id_type entityId, const component_type& component)
        {
            component_type temp = component;
            return force_value_cast<component_handle<component_type>>(createComponent(entityId, typeHash<component_type>(), &temp));
        }

        template<typename archetype_type, typename = inherits_from<archetype_type, archetype_base>>
        auto createComponent(id_type entityId)
        {
            return archetype_type::create(this, entityId);
        }

        template<typename... component_types>
        std::tuple<component_handle<component_types>...> createComponents(id_type entityId)
        {
            return std::make_tuple(createComponent<component_types>(entityId)...);
        }

		/**@brief Create component of a certain type attached to a certain entity.
		 * @param entityId Id of the entity you wish to attach the component to.
		 * @param componentTypeId Type id of component you wish to create.
		 * @returns component_handle_base Component handle to the created component.
		 * @note Handle needs to force_cast to component_handle<T> in order to use correctly.
		 * @throws args_entity_not_found_error If the entity id does not belong to a valid entity.
		 */
		component_handle_base createComponent(id_type entityId, id_type componentTypeId);

		component_handle_base createComponent(id_type entityId, id_type componentTypeId, void* value);

		/**@brief Destroy component of a certain type attached to a certain entity.
		 * @tparam component_type Type of component you wish to destroy.
		 * @param entityId Id of the entity you wish to remove the component from.
		 * @throws args_entity_not_found_error If the entity id does not belong to a valid entity.
		 */
		template<typename component_type>
		void destroyComponent(id_type entityId)
		{
			destroyComponent(entityId, typeHash<component_type>());
		}

		/**@brief Destroy component of a certain type attached to a certain entity.
		 * @param entityId Id of the entity you wish to remove the component from.
		 * @param componentTypeId Type id of component you wish to destroy.
		 * @throws args_entity_not_found_error If the entity id does not belong to a valid entity.
		 */
		void destroyComponent(id_type entityId, id_type componentTypeId);

		/**@brief Check if entity exists.
		 * @param entityId Id of entity you wish to check if it exists.
		 * @returns bool True if entity exists, false if it doesn't or if the id is invalid_id.
		 */
		A_NODISCARD bool validateEntity(id_type entityId);

		/**@brief Create new entity.
		 * @returns entity_handle Entity handle pointing to the newly created entity.
		 * @throws args_entity_exists_error When the next entity id is somehow already taken. (only possible if someone else messed with my/Glyn's code)
		 */
		entity_handle createEntity();

		/**@brief Destroys entity and all of its components.
		 * @param entityId Id of entity you wish to destroy.
		 * @param recurse Do you wish to destroy all children and children of children etc as well? True by default.
		 */
		void destroyEntity(id_type entityId, bool recurse = true);

		/**@brief Get entity handle for a certain entity id.
		 * @param entityId Id of entity you want a handle to.
		 * @returns entity_handle Handle to the requested entity. (may be invalid if the entity doesn't exist)
		 * @note You could always use the entity_handle constructor... you have the registry reference already anyways.
		 */
		A_NODISCARD entity_handle getEntity(id_type entityId);

		/**@brief Get entity data for a certain entity id.
		 * @param entityId Id of entity you want the data from.
		 * @returns entity_data& Hierarchy and composition data of the entity requested.
		 */
		A_NODISCARD entity_data& getEntityData(id_type entityId);

		/**@brief Get a container with ALL entities.
		 * @returns sparse_map<id_type, entity_handle>& Container that keeps both the id's and corresponding entity handles for easy use.
		 */
		A_NODISCARD std::pair<entity_set&, async::readonly_rw_spinlock&>  getEntities();

		/**@brief Get a query for your component combination.
		 * @tparam component_types Variadic parameter types of all component types you wish to query for.
		 * @returns EntityQuery Query that will query the entities with the requested components.
		 * @note This function is quite slow and thus it's advised to create a query once and reuse it.
		 */
		template<typename... component_types>
		A_NODISCARD EntityQuery createQuery()
		{
			return m_queryRegistry.createQuery<component_types...>();
		}

		/**@brief Get a query for your component combination.
		 * @param componentTypes Container with type ids of all component types you wish to query for.
		 * @returns EntityQuery Query that will query the entities with the requested components.
		 * @note This function is quite slow and thus it's advised to create a query once and reuse it.
		 */
		A_NODISCARD EntityQuery createQuery(const hashed_sparse_set<id_type>& componentTypes)
		{
			return m_queryRegistry.createQuery(componentTypes);
		}
	};
}

#include <core/ecs/archetype.inl>
