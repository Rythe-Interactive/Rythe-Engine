#pragma once
#include <core/containers/containers.hpp>
#include <core/types/types.hpp>
#include <core/common/common.hpp>
#include <core/async/async.hpp>
#include <core/ecs/component_pool.hpp>
#include <core/ecs/queryregistry.hpp>
#include <core/ecs/entityquery.hpp>
#include <core/ecs/entity_handle.hpp>
#include <core/ecs/archetype.hpp>

#include <utility>
#include <memory>
#include <unordered_map>

/**
 * @file ecsregistry.hpp
 */

namespace legion::core::ecs
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
    class EcsRegistry
    {
    private:
        static id_type m_nextEntityId;

        mutable async::rw_spinlock m_familyLock;
        std::unordered_map<id_type, std::unique_ptr<component_pool_base>> m_families;
        std::unordered_map<id_type,std::string> m_prettyNames;

        mutable async::rw_spinlock m_entityDataLock;
        std::unordered_map<id_type, entity_data> m_entityData;

        mutable async::rw_spinlock m_entityLock;
        entity_set m_entities;

        QueryRegistry m_queryRegistry;
        events::EventBus* m_eventBus;

        /**@brief Internal function for recursively destroying all children and children of children etc.
         */
        void recursiveDestroyEntityInternal(id_type entityId);

    public:
        static entity_handle world;

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
            OPTICK_EVENT();
            async::readwrite_guard guard(m_familyLock);
            if (!m_families.count(typeHash<component_type>())) {
                m_families[typeHash<component_type>()] = std::make_unique<component_pool<component_type>>(this, m_eventBus);
                m_prettyNames[typeHash<component_type>()] = std::string(typeid(component_type).name());
            }
        }

        /**@brief Get component storage of a certain type.
         * @tparam component_type Type of the component you wish to fetch.
         * @returns component_pool<component_type>* Pointer to the component container that contains all components of the requested type.
         * @throws legion_unknown_component_error When component type is unknown.
         */
        template<typename component_type>
        L_NODISCARD component_pool<component_type>* getFamily()
        {
            return static_cast<component_pool<component_type>*>(getFamily(typeHash<component_type>()));
        }

        async::rw_spinlock& getEntityLock() const 
        {
            return m_entityLock;
        }

        /**@brief  TODO*/
        std::string getFamilyName(id_type id)
        {
            if(const auto itr = m_prettyNames.find(id);itr != m_prettyNames.end())
            {
                return itr->second;
            }
            return "";
        }

        /**@brief Get component storage of a certain type.
         * @param componentTypeId Type id of the component you wish to fetch.
         * @returns component_pool_base* Pointer to the component container that contains all components of the requested type. (needs to be cast to original type to use)
         * @throws legion_unknown_component_error When component type is unknown.
         */
        L_NODISCARD component_pool_base* getFamily(id_type componentTypeId);

        /**@brief Check if an entity has a certain component.
         * @param entityId Id of the entity.
         * @param componentTypeId Type id of component to check for.
         * @throws legion_entity_not_found_error If the entity id does not belong to a valid entity.
         */
        L_NODISCARD bool hasComponent(id_type entityId, id_type componentTypeId);

        /**@brief Check if an entity has a certain component.
         * @tparam component_type Type of the component to check for.
         * @param entityId Id of the entity.
         * @throws legion_entity_not_found_error If the entity id does not belong to a valid entity.
         */
        template<typename component_type, doesnt_inherit_from<component_type, archetype_base> = 0>
        L_NODISCARD bool hasComponent(id_type entityId)
        {
            return hasComponent(entityId, typeHash<component_type>());
        }

        /**@brief Check if an entity has a certain component combination.
         * @tparam component_type Type of the first component to check for.
         * @tparam component_types Types of the other components to check for.
         * @param entityId Id of the entity.
         * @throws legion_entity_not_found_error If the entity id does not belong to a valid entity.
         */
        template<typename component_type, typename... component_types, doesnt_inherit_from<component_type, archetype_base> = 0>
        L_NODISCARD bool hasComponents(id_type entityId)
        {
            return hasComponent<component_type>(entityId) && (hasComponent<component_types>(entityId) && ...);
        }

        /**@brief Check if an entity has a certain component combination using an archetype.
         * @tparam archetype_type Type of the archetype to check for.
         * @param entityId Id of the entity.
         * @throws legion_entity_not_found_error If the entity id does not belong to a valid entity.
         */
        template<typename archetype_type, inherits_from<archetype_type, archetype_base> = 0>
        L_NODISCARD bool hasComponents(id_type entityId)
        {
            return archetype_type::has(this, entityId);
        }

        /**@brief Get component handle of a certain type attached to a certain entity.
         * @param entityId Id of entity to get the component from.
         * @param componentTypeId Type id of component to get handle of.
         * @returns component_handle_base Component handle to the requested component. (may be invalid if the entity does not have this component type)
         * @note Handle needs to force_cast to component_handle<T> in order to use correctly.
         * @throws legion_entity_not_found_error If the entity id does not belong to a valid entity.
         */
        L_NODISCARD component_handle_base getComponent(id_type entityId, id_type componentTypeId);

        /**@brief Get component handle of a certain type attached to a certain entity.
         * @tparam component_type Type of component to get handle of.
         * @param entityId Id of entity to get the component from.
         * @returns component_handle<component_type> Component handle to the requested component. (may be invalid if the entity does not have this component type)
         * @throws legion_entity_not_found_error If the entity id does not belong to a valid entity.
         */
        template<typename component_type, doesnt_inherit_from<component_type, archetype_base> = 0>
        L_NODISCARD component_handle<component_type> getComponent(id_type entityId)
        {
            return getComponent(entityId, typeHash<component_type>()).template cast<component_type>();
        }

        /**@brief Get component handles of a certain types attached to a certain entity.
         * @tparam component_type Type of the first component to get handle of.
         * @tparam component_types Types of the other components to get the handles of.
         * @param entityId Id of entity to get the component from.
         * @returns component_handle<component_type> Component handle to the requested component. (may be invalid if the entity does not have this component type)
         * @throws legion_entity_not_found_error If the entity id does not belong to a valid entity.
         */
        template<typename component_type, typename... component_types, doesnt_inherit_from<component_type, archetype_base> = 0>
        L_NODISCARD std::tuple<component_handle<component_type>, component_handle<component_types>...> getComponents(id_type entityId)
        {
            return std::make_tuple(getComponent<component_type>(entityId), getComponent<component_types>(entityId)...);
        }

        template<typename archetype_type, typename... component_types, inherits_from<archetype_type, archetype_base> = 0>
        L_NODISCARD auto getComponents(id_type entityId)
        {
            return archetype_type::get(this, entityId);
        }

        /**@brief Create component of a certain type attached to a certain entity.
         * @tparam component_type Type of component you wish to create.
         * @param entityId Id of the entity you wish to attach the component to.
         * @returns component_handle<component_type> Component handle to the created component.
         * @throws legion_entity_not_found_error If the entity id does not belong to a valid entity.
         */
        template<typename component_type, doesnt_inherit_from<component_type, archetype_base> = 0>
        component_handle<component_type> createComponent(id_type entityId)
        {
            return createComponent(entityId, typeHash<component_type>()).template cast<component_type>();
        }

        template<typename component_type, doesnt_inherit_from<component_type, archetype_base> = 0>
        component_handle<std::remove_reference_t<component_type>> createComponent(id_type entityId, component_type&& component)
        {
            std::remove_reference_t<component_type> temp = component;
            return createComponent(entityId, typeHash<std::remove_reference_t<component_type>>(), &temp).template cast<std::remove_reference_t<component_type>>();
        }

        template<typename component_type, doesnt_inherit_from<component_type, archetype_base> = 0>
        component_handle<std::remove_reference_t<component_type>> createComponent(id_type entityId, component_type& component)
        {
            std::remove_reference_t<component_type> temp = component;
            return createComponent(entityId, typeHash<std::remove_reference_t<component_type>>(), &temp).template cast<std::remove_reference_t<component_type>>();
        }

        template<typename archetype_type, inherits_from<archetype_type, archetype_base> = 0>
        auto createComponents(id_type entityId)
        {
            return archetype_type::create(this, entityId);
        }

        template<typename archetype_type, typename... component_types, inherits_from<archetype_type, archetype_base> = 0>
        auto createComponents(id_type entityId, component_types&&... defaultValues)
        {
            return archetype_type::create(this, entityId, std::move(defaultValues)...);
        }

        template<typename component_type, typename... component_types, doesnt_inherit_from<component_type, archetype_base> = 0>
        std::tuple<component_handle<component_type>, component_handle<component_types>...> createComponents(id_type entityId)
        {
            return std::make_tuple(createComponent<component_type>(entityId), createComponent<component_types>(entityId)...);
        }

        template<typename component_type, typename... component_types, doesnt_inherit_from<component_type, archetype_base> = 0>
        std::tuple<component_handle<std::remove_reference_t<component_type>>, component_handle<std::remove_reference_t<component_types>>...> createComponents(id_type entityId, component_type&& defaultValue, component_types&&... defaultValues)
        {
            return std::make_tuple(createComponent<std::remove_reference_t<component_type>>(entityId, std::forward<component_type>(defaultValue)), createComponent<std::remove_reference_t<component_types>>(entityId, std::forward<component_types>(defaultValues))...);
        }

        /**@brief Create component of a certain type attached to a certain entity.
         * @param entityId Id of the entity you wish to attach the component to.
         * @param componentTypeId Type id of component you wish to create.
         * @returns component_handle_base Component handle to the created component.
         * @note Handle needs to force_cast to component_handle<T> in order to use correctly.
         * @throws legion_entity_not_found_error If the entity id does not belong to a valid entity.
         */
        component_handle_base createComponent(id_type entityId, id_type componentTypeId);

        /**@brief Create component of a certain type attached to a certain entity.
         * @param entityId Id of the entity you wish to attach the component to.
         * @param componentTypeId Type id of component you wish to create.
         * @param value Pointer to component_type that has the starting value you require.
         * @returns component_handle_base Component handle to the created component.
         * @note Handle needs to force_cast to component_handle<T> in order to use correctly.
         * @throws legion_entity_not_found_error If the entity id does not belong to a valid entity.
         */
        component_handle_base createComponent(id_type entityId, id_type componentTypeId, void* value);

        /**
         * @brief Copies a component from entity to another.
         * @param destinationEntity The entity you want to copy the component onto.
         * @param sourceEntity The entity you want to copy the component from.
         * @param componentTypeId The component you want to copy
         * @return component_handle_base Component handle to the copied component.
        */
        component_handle_base copyComponent(id_type destinationEntity, id_type sourceEntity, id_type componentTypeId);

        /**@brief Destroy component of a certain type attached to a certain entity.
         * @param entityId Id of the entity you wish to remove the component from.
         * @param componentTypeId Type id of component you wish to destroy.
         * @throws legion_entity_not_found_error If the entity id does not belong to a valid entity.
         */
        void destroyComponent(id_type entityId, id_type componentTypeId);

        /**@brief Destroy component of a certain type attached to a certain entity.
         * @tparam component_type Type of component you wish to destroy.
         * @param entityId Id of the entity you wish to remove the component from.
         * @throws legion_entity_not_found_error If the entity id does not belong to a valid entity.
         */
        template<typename component_type, doesnt_inherit_from<component_type, archetype_base> = 0>
        void destroyComponent(id_type entityId)
        {
            destroyComponent(entityId, typeHash<component_type>());
        }

        template<typename component_type, typename... component_types, doesnt_inherit_from<component_type, archetype_base> = 0>
        void destroyComponents(id_type entityId)
        {
            destroyComponent(entityId, typeHash<component_type>());
            (destroyComponent(entityId, typeHash<component_types>()), ...);
        }

        template<typename archetype_type, inherits_from<archetype_type, archetype_base> = 0>
        void destroyComponents(id_type entityId)
        {
            archetype_type::destroy(this, entityId);
        }

        /**@brief Check if entity exists.
         * @param entityId Id of entity you wish to check if it exists.
         * @returns bool True if entity exists, false if it doesn't or if the id is invalid_id.
         */
        L_NODISCARD bool validateEntity(id_type entityId);

        /**@brief Create new entity.
         * @returns entity_handle Entity handle pointing to the newly created entity.
         */
        L_NODISCARD entity_handle createEntity(id_type entityId = invalid_id);

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
        L_NODISCARD entity_handle getEntity(id_type entityId);

        /**@brief Get entity data for a certain entity id.
         * @param entityId Id of entity you want the data from.
         * @returns entity_data& Hierarchy and composition data of the entity requested.
         */
        L_NODISCARD entity_data getEntityData(id_type entityId);
        void setEntityData(id_type entityId, const entity_data& data);

        L_NODISCARD entity_handle getEntityParent(id_type entityId);

        /**@brief Get a container with ALL entities.
         * @returns sparse_map<id_type, entity_handle>& Container that keeps both the id's and corresponding entity handles for easy use.
         */
        L_NODISCARD std::pair<entity_set&, async::rw_spinlock&> getEntities();

        /**@brief Get a query for your component combination.
         * @tparam component_types Variadic parameter types of all component types you wish to query for.
         * @returns EntityQuery Query that will query the entities with the requested components.
         * @note This function is quite slow and thus it's advised to create a query once and reuse it.
         */
        template<typename... component_types>
        L_NODISCARD EntityQuery createQuery()
        {
            return m_queryRegistry.createQuery<component_types...>();
        }

        /**@brief Get a query for your component combination.
         * @param componentTypes Container with type ids of all component types you wish to query for.
         * @returns EntityQuery Query that will query the entities with the requested components.
         * @note This function is quite slow and thus it's advised to create a query once and reuse it.
         */
        L_NODISCARD EntityQuery createQuery(const hashed_sparse_set<id_type>& componentTypes)
        {
            return m_queryRegistry.createQuery(componentTypes);
        }
    };
}

#include <core/ecs/entity_handle.inl>
#include <core/ecs/archetype.inl>
