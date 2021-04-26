#pragma once
#include <unordered_set>
#include <unordered_map>
#include <memory>

#include <Optick/optick.h>

#include <core/containers/sparse_map.hpp>
#include <core/containers/hashed_sparse_set.hpp>
#include <core/types/types.hpp>
#include <core/platform/platform.hpp>
#include <core/common/hash.hpp>

#include <core/ecs/containers/component_pool.hpp>
#include <core/ecs/handles/entity.hpp>
#include <core/ecs/handles/component.hpp>
#include <core/ecs/data/entity_data.hpp>
#include <core/ecs/prototypes/entity_prototype.hpp>
#include <core/ecs/meta/meta.hpp>

/**
 * @file registry.hpp
 */

namespace legion::core::ecs
{
    /**@class Registry
     * @brief Manager and owner of all component families and entities.
     */
    class Registry
    {
    private:
        // All miscellaneous data on entities, eg: hierarchy, active, alive.
        static std::unordered_map<id_type, entity_data> m_entities;

        // All recyclable entities that are dead.
        static std::queue<id_type> m_recyclableEntities;

        static std::unordered_map<id_type, std::string>& familyNames();

        /**@brief Inserts in-place if the component family does not exist, returns existing item if the family exists.
         * @param args Arguments to forward to the constructor of the component family.
         * @return Pointer to the newly inserted family if one didn't exist. Pointer to the existing item if one did exist.
         */
        template<typename component_type, typename... Args>
        static component_pool<component_type>* tryEmplaceFamily(Args&&... args);

        L_NODISCARD static id_type getNextEntityId();

    public:
        static void clear();

        /**@brief Creates world entity if it doesn't exist yet and returns it.
         * @note The world entity can also be retrieved using `ecs::world` and
         *       doesn't require a check for whether the world already exists or not.
         * @return World entity.
         */
        L_NODISCARD static entity getWorld();

        /**@brief Manually registers a component type. Allows use of non templated functions for this component type.
         * @note This function generally not necessary to call manually. All templated functions will try to register
         *       an unknown component type before they attempt their operation.
         * @note For a struct to work as a component it needs to be default constructible.
         * @tparam component_type Type of struct you with to register as a component.
         */
        template<typename component_type, typename... Args>
        static void registerComponentType(Args&&... args);

        /**@brief Manually registers a component type. Allows use of non templated functions for this component type.
         * @note This function generally not necessary to call manually. All templated functions will try to register
         *       an unknown component type before they attempt their operation.
         * @note For a struct to work as a component it needs to be default constructible.
         * @tparam component_type Type of struct you with to register as a component.
         */
        template<typename component_type0, typename component_type1, typename... component_types, typename... Args>
        static void registerComponentType(Args&&... args);

        /**@brief Get a pointer to the family of a certain component type.
         */
        template<typename component_type, typename... Args>
        L_NODISCARD static component_pool<component_type>* getFamily(Args&&... args);

        /**@brief Non templated way to get a pointer to the family of a certain component type.
         * @note Will throw an exception if non of the templated functions have been called
         *       before for this component type due to the type not being registered yet.
         * @param typeId Local type hash of the component type.
         */
        L_NODISCARD static component_pool_base* getFamily(id_type typeId);

        L_NODISCARD static std::string getFamilyName(id_type id);

        /**@brief Gets the container with all the component storage families.
         */
        L_NODISCARD static std::unordered_map<id_type, std::unique_ptr<component_pool_base>>& getFamilies();

        /**@brief Creates empty entity with the world as its parent.
         */
        L_NODISCARD static entity createEntity();

        L_NODISCARD static entity createEntity(const std::string& name);

        /**@brief Creates empty entity with a specific entity as its parent.
         * @param parent Entity to assign as the parent of the new entity.
         */
        L_NODISCARD static entity createEntity(entity parent);

        L_NODISCARD static entity createEntity(const std::string& name, entity parent);

        /**@brief Creates empty entity with a specific entity as its parent. Entity is serialized from a prototype.
         *        This function will also create any components or child entities in the prototype structure.
         * @param parent Entity to assign as the parent of the new entity.
         * @param prototype Prototype to serialize entity from. 
         */
        static entity createEntity(entity parent, const serialization::entity_prototype& prototype);

        /**@brief Creates empty entity with the world as its parent. Entity is serialized from a prototype.
         *        This function will also create any components or child entities in the prototype structure.
         * @param prototype Prototype to serialize entity from.
         */
        static entity createEntity(const serialization::entity_prototype& prototype);

        /**@brief Destroys an entity and all its components.
         * @param target Entity to destroy.
         * @param recurse Whether to recursively destroy all the child entities as well. True by default.
         */
        static void destroyEntity(entity target, bool recurse = true);
        static void destroyEntity(id_type target, bool recurse = true);

        /**@brief Checks if an entity exists and is alive.
         * @param target Entity to check.
         * @return True if the entity is valid and alive, false if the entity is dead or non existent.
         */
        L_NODISCARD static bool checkEntity(entity target);
        L_NODISCARD static bool checkEntity(id_type target);

        /**@brief Gets the component compositions of all entities.
         */
        L_NODISCARD static std::unordered_map<id_type, std::unordered_set<id_type>>& entityCompositions() noexcept;

        /**@brief Gets the entity specific data of all entities.
         */
        L_NODISCARD static std::unordered_map<id_type, entity_data>& entityData();

        /**@brief Gets the component composition of a specific entity.
         */
        L_NODISCARD static std::unordered_set<id_type>& entityComposition(entity target);
        L_NODISCARD static std::unordered_set<id_type>& entityComposition(id_type target);

        /**@brief Gets the entity specific data of a specific entity.
         */
        L_NODISCARD static entity_data& entityData(id_type target);
        L_NODISCARD static entity getEntity(id_type target);

        /**@brief Creates a new component of a certain type for a specific entity.
         * @tparam component_type Type of component to create.
         * @param target Entity to create the component for.
         * @return Reference to the created component.
         */
        template<typename component_type>
        static component_ref_t<component_type> createComponent(entity target);

        template<typename component_type0, typename component_type1, typename... component_typeN>
        static component_ref_tuple<component_type0, component_type1, component_typeN...> createComponent(entity target);

        /**@brief Adds a precreated component of a certain type to a specific entity.
         * @tparam component_type Type of component to create.
         * @param target Entity to create the component for.
         * @param value Component value to use.
         * @return Reference to the created component.
         */
        template<typename component_type>
        static component_ref_t<component_type> createComponent(entity target, component_type&& value);
        template<typename component_type>
        static component_ref_t<component_type> createComponent(entity target, const component_type& value);

        template<typename archetype_type, typename component_type0, typename component_type1, typename... component_typeN>
        static typename archetype_type::ref_group createComponent(entity target, component_type0&& value0, component_type1&& value1, component_typeN&&... valueN);

        template<typename component_type0, typename component_type1, typename... component_typeN>
        static component_ref_tuple<component_type0, component_type1, component_typeN...> createComponent(entity target, component_type0&& value0, component_type1&& value1, component_typeN&&... valueN);
        template<typename component_type0, typename component_type1, typename... component_typeN>
        static component_ref_tuple<component_type0, component_type1, component_typeN...> createComponent(entity target, const component_type0& value0, const component_type1& value1, const component_typeN&... valueN);

        /**@brief Creates a new component of a certain type for a specific entity. Component is serialized from a prototype.
         * @tparam component_type Type of component to create.
         * @param target Entity to create the component for.
         * @param prototype Prototype to serialize component from.
         * @return Reference to the created component.
         */
        template<typename component_type>
        static component_type& createComponent(entity target, const serialization::component_prototype<component_type>& prototype);
        template<typename component_type>
        static component_type& createComponent(entity target, serialization::component_prototype<component_type>&& prototype);

        /**@brief Creates a new component of a certain type for a specific entity.
         * @param typeId Type hash of component type to create.
         * @param target Entity to create the component for.
         * @return Pointer to the created component.
         */
        static void* createComponent(id_type typeId, entity target);

        /**@brief Creates a new component of a certain type for a specific entity. Component is serialized from a prototype.
         * @param typeId Type hash of component type to create.
         * @param target Entity to create the component for.
         * @param prototype Prototype to serialize component from.
         * @return Pointer to the created component.
         */
        static void* createComponent(id_type typeId, entity target, const serialization::component_prototype_base& prototype);
        static void* createComponent(id_type typeId, entity target, serialization::component_prototype_base&& prototype);

        /**@brief Destroys a certain component on a specific entity.
         * @tparam component_type Type of the component to destroy.
         * @param target Entity to destroy and remove the component from.
         */
        template<typename component_type>
        static void destroyComponent(entity target);

        template<typename component_type0, typename component_type1, typename... component_typeN>
        static void destroyComponent(entity target);

        /**@brief Destroys a certain component on a specific entity.
         * @param typeId Type hash of component type to destroy.
         * @param target Entity to destroy and remove the component from.
         */
        static void destroyComponent(id_type typeId, entity target);

        /**@brief Checks if a specific entity has a certain component.
         * @tparam component_type Type of the component to check for.
         * @param target Entity to check the component for.
         * @return True if the target has the component, false if not.
         */
        template<typename component_type>
        L_NODISCARD static bool hasComponent(entity target);

        template<typename component_type0, typename component_type1, typename... component_typeN>
        L_NODISCARD static bool hasComponent(entity target);

        /**@brief Checks if a specific entity has a certain component.
         * @param typeId Type hash of the component type to check for.
         * @param target Entity to check the component for.
         * @return True if the target has the component, false if not.
         */
        L_NODISCARD static bool hasComponent(id_type typeId, entity target);

        /**@brief Gets a reference to an existing component.
         * @tparam component_type Type of the component to fetch.
         * @param target Entity to get the component from.
         * @return Reference to the component.
         */
        template<typename component_type>
        L_NODISCARD static component_ref_t<component_type> getComponent(entity target);

        template<typename component_type0, typename component_type1, typename... component_typeN>
        L_NODISCARD static component_ref_tuple<component_type0, component_type1, component_typeN... > getComponent(entity target);

        /**@brief Gets a pointer to an existing component.
         * @param typeId Type hash of the component to fetch.
         * @param target Entity to get the component from.
         * @return Pointer to the component.
         */
        L_NODISCARD static void* getComponent(id_type typeId, entity target);
    };

    /**@brief World entity. All entities and scenes are eventually parented to this entity.
     */
    extern entity world;
}

#include <core/ecs/registry.inl>
#include <core/ecs/containers/component_pool.inl>
#include <core/ecs/filters/filterregistry.inl>
#include <core/ecs/handles/entity.inl>
#include <core/ecs/handles/component.inl>
#include <core/ecs/prototypes/entity_prototype.inl>
#include <core/ecs/archetype/archetype.inl>

