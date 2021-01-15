#pragma once
#include <core/types/types.hpp>
#include <core/containers/sparse_map.hpp>
#include <core/containers/hashed_sparse_set.hpp>
#include <core/platform/platform.hpp>
#include <core/ecs/archetype.hpp>
#include <memory>
#include <cereal/types/vector.hpp>
#include <cereal/archives/binary.hpp>
#include <cereal/archives/json.hpp>
#include <core/events/eventbus.hpp>

/**
 * @file entity_handle.hpp
 */

namespace legion::core::ecs
{
    class EcsRegistry;

    class component_handle_base;

    template<typename component_type>
    class component_handle;

    class entity_handle;

    /**@class entity_handle
     * @brief Serializable handle for executing operations on entities.
     *		  This class only stores a reference to the registry and the id of the entity.
     */
    class entity_handle
    {
        friend class EcsRegistry;
    private:
        id_type m_id;
        static EcsRegistry* m_registry;
        static events::EventBus* m_eventBus;
        using entity_set = hashed_sparse_set<entity_handle>;

    public:
        /**@brief Main constructor for constructing a valid entity handle.
         */
        entity_handle(id_type id) noexcept : m_id(id) {  }

        /**@brief Constructor for constructing an invalid entity handle.
         * @note Should only be used to create temporary handles. Allows use of entity handle in containers together with copy constructor.
         */
        entity_handle() noexcept : m_id(invalid_id) {  }

        /**@brief Copy constructor (DOES NOT CREATE NEW ENTITY, both handles will reference the same entity).
         * @note Allows use of entity handle in containers together with default invalid entity constructor.
         */
        entity_handle(const entity_handle& other) noexcept : m_id(other.m_id) {  }

        /**@brief Copy assignment. Exists for the same reasons as the copy constructor.
         * @ref legion::core::ecs::entity_handle::entity_handle(const legion::core::ecs::entity& other)
         */
        entity_handle& operator=(const entity_handle& other) noexcept;

        /**
         * @brief Clones an entity.
         * @param keep_parent If true sets the parent of the cloned entity to the same parent as the source [default: true]
         * @param clone_children  If true copies all the children of the entity as well. [default: true]
         * @param clone_components  If true copies all the components of the entity. [default: true]
         * @return entity_handle A handle to the copied entity.
        */
        L_NODISCARD entity_handle clone(bool keep_parent = true, bool clone_children = true, bool clone_components = true) const;

        /**@brief Returns the type id's of the components this entity contains.
         * @returns hashed_sparse_set<id_type>& Sparse map with component type id as both the key as well as the value. (behaves as sparse_set with hash table)
         * @throws legion_invalid_entity_error Thrown when handle's registry reference is invalid.
         * @throws legion_entity_not_found_error Thrown when handle's id is invalid.
         */
        L_NODISCARD hashed_sparse_set<id_type> component_composition() const;

        /**@brief Cast to id_type returns the id of the entity this handle references.
         * @returns id_type If the handle is valid it will return the entity id, otherwise invalid_id.
         */
        operator id_type() const { return m_id; }

        bool operator==(const entity_handle& other) const
        {
            return m_id == other.m_id;
        }

        operator bool() const
        {
            return valid();
        }

        /**@brief Returns the id of the entity this handle references.
         * @returns id_type If the handle is valid it will return the entity id, otherwise invalid_id.
         */
        L_NODISCARD id_type get_id() const;

        /**@brief Returns hashed sparse set with all children of this entity.
         */
        L_NODISCARD entity_set children() const;

        /**@brief Returns entity handle to parent entity.
         * @returns entity_handle Entity handle that either points to the parent entity or is invalid if the entity doesn't have a parent.
         * @throws legion_invalid_entity_error Thrown when handle's registry reference is invalid.
         * @throws legion_entity_not_found_error Thrown when handle's id is invalid.
         */
        L_NODISCARD entity_handle get_parent() const;

        /**@brief Set parent of entity.
         * @param newParent Id of the entity you wish to set as the parent. (invalid_id if you wish to remove parent)
         * @note Will remove as child from parent.
         * @throws legion_invalid_entity_error Thrown when handle's registry reference is invalid.
         * @throws legion_entity_not_found_error Thrown when handle's id is invalid.
         */
        void set_parent(id_type newParent);

        /**@brief serializes the entity depending on its archive
         * @param oarchive template<typename Archive>
         * @note Will only be called when said entity is serializes through an archive.
         */
        void serialize(cereal::JSONOutputArchive& oarchive);
        void serialize(cereal::BinaryOutputArchive& oarchive);
        void serialize(cereal::JSONInputArchive& oarchive);
        void serialize(cereal::BinaryInputArchive& oarchive);

        /**@brief Get child of the entity at a certain index.
         * @throws std::out_of_range Thrown when index is more than or equal to the child count.
         * @throws legion_invalid_entity_error Thrown when handle's registry reference is invalid.
         * @throws legion_entity_not_found_error Thrown when handle's id is invalid.
         */
        L_NODISCARD entity_handle operator[] (index_type index) const;

        /**@brief Get child of the entity at a certain index.
         * @throws std::out_of_range Thrown when index is more than or equal to the child count.
         * @throws legion_invalid_entity_error Thrown when handle's registry reference is invalid.
         * @throws legion_entity_not_found_error Thrown when handle's id is invalid.
         */
        L_NODISCARD entity_handle get_child(index_type index) const;

        /**@brief Get amount of children the referenced entity has.
         * @returns size_type
         * @throws legion_invalid_entity_error Thrown when handle's registry reference is invalid.
         * @throws legion_entity_not_found_error Thrown when handle's id is invalid.
         */
        L_NODISCARD size_type child_count() const;

        /**@brief Add child to entity.
         * @param childId Id of the entity you wish to add as a child.
         * @note Will change parent of the entity to add as child.
         * @throws legion_invalid_entity_error Thrown when handle's registry reference is invalid.
         * @throws legion_entity_not_found_error Thrown when handle's id is invalid.
         */
        void add_child(id_type childId);

        /**@brief Remove child from entity.
         * @param childId Id of the entity you wish to remove as a child.
         * @note Will remove parent of the entity to remove as child.
         * @throws legion_invalid_entity_error Thrown when handle's registry reference is invalid.
         * @throws legion_entity_not_found_error Thrown when handle's id is invalid.
         */
        void remove_child(id_type childId);

        /**@brief Check if entity contains a certain component.
         * @tparam component_type Type of component to check for.
         * @throws legion_invalid_entity_error Thrown when handle's registry reference is invalid.
         * @throws legion_entity_not_found_error Thrown when handle's id is invalid.
         */
        template<typename component_type>
        L_NODISCARD bool has_component() const { return has_component(typeHash<component_type>()); }

        /**@brief Check if entity contains a certain components.
         * @tparam component_type First type of component to check for.
         * @tparam component_types The rest of the types of components to check for.
         * @throws legion_invalid_entity_error Thrown when handle's registry reference is invalid.
         * @throws legion_entity_not_found_error Thrown when handle's id is invalid.
         */
        template<typename component_type, typename... component_types, typename = doesnt_inherit_from<component_type, archetype_base>>
        L_NODISCARD bool has_components() const;

        /**@brief Check if entity contains the components of a certain archetype.
         * @tparam archetype_type The type of archetype to check for.
         * @throws legion_invalid_entity_error Thrown when handle's registry reference is invalid.
         * @throws legion_entity_not_found_error Thrown when handle's id is invalid.
         */
        template<typename archetype_type, typename = inherits_from<archetype_type, archetype_base>>
        L_NODISCARD bool has_components() const;

        /**@brief Check if entity contains a certain component.
         * @param componentTypeId Type id of component to check for.
         * @throws legion_invalid_entity_error Thrown when handle's registry reference is invalid.
         * @throws legion_entity_not_found_error Thrown when handle's id is invalid.
         */
        L_NODISCARD bool has_component(id_type componentTypeId) const;

        /**@brief Get component handle of a certain component.
         * @param componentTypeId Type id of component to fetch.
         * @throws legion_invalid_entity_error Thrown when handle's registry reference is invalid.
         * @throws legion_entity_not_found_error Thrown when handle's id is invalid.
         * @returns component_handle_base Valid component handle if the entity has the component, invalid handle if the entity doesn't have the component.
         * @note component_handle_base needs to be force_cast to component_handle<T> in order to be usable.
         */
        L_NODISCARD component_handle_base get_component_handle(id_type componentTypeId);
        L_NODISCARD const component_handle_base get_component_handle(id_type componentTypeId) const;

        /**@brief Get component handle of a certain component.
         * @tparam component_type Type of component to fetch.
         * @throws legion_invalid_entity_error Thrown when handle's registry reference is invalid.
         * @throws legion_entity_not_found_error Thrown when handle's id is invalid.
         * @returns component_handle<component_type> Valid component handle if the entity has the component, invalid handle if the entity doesn't have the component.
         */
        template<typename component_type>
        L_NODISCARD component_handle<component_type> get_component_handle()
        {
            return get_component_handle(typeHash<component_type>()).template cast<component_type>();
        }
        
        template<typename component_type>
        L_NODISCARD const component_handle<component_type> get_component_handle() const
        {
            return get_component_handle(typeHash<component_type>()).template cast<component_type>();
        }

        /**@brief Get component handles of certain components.
         * @tparam component_type First type of component to fetch.
         * @tparam component_types The rest of the types of components to fetch.
         * @throws legion_invalid_entity_error Thrown when handle's registry reference is invalid.
         * @throws legion_entity_not_found_error Thrown when handle's id is invalid.
         * @returns Tuple with all the handles.
         */
        template<typename component_type, typename... component_types, typename = doesnt_inherit_from<component_type, archetype_base>>
        L_NODISCARD auto get_component_handles() const;

        /**@brief Get component handles of all components of a certain archetype.
         * @tparam archetype_type The type of archetype to fetch the components of.
         * @throws legion_invalid_entity_error Thrown when handle's registry reference is invalid.
         * @throws legion_entity_not_found_error Thrown when handle's id is invalid.
         * @returns Tuple with all the handles.
         */
        template<typename archetype_type, typename = inherits_from<archetype_type, archetype_base>>
        L_NODISCARD auto get_component_handles() const;

        /**@brief Shortcut to component_handle::read
         */
        template<typename component_type>
        L_NODISCARD component_type read_component() const
        {
            return get_component_handle<component_type>().read();
        }

        /**@brief Shortcut to component_handle::write
         */
        template<typename component_type>
        void write_component(component_type&& value)
        {
            get_component_handle<std::remove_reference_t<component_type>>().write(std::forward<component_type>(value));
        }
        /**@brief Shortcut to component_handle::write
         */
        template<typename component_type>
        void write_component(const component_type& value)
        {
            get_component_handle<std::remove_reference_t<component_type>>().write(std::forward<component_type>(value));
        }

        /**@brief Add component to the entity.
         * @param componentTypeId Type id of component to add.
         * @throws legion_invalid_entity_error Thrown when handle's registry reference is invalid.
         * @throws legion_entity_not_found_error Thrown when handle's id is invalid.
         * @throws legion_unknown_component_error Thrown when the component type is unknown.
         * @returns component_handle_base Valid component handle base for the newly created component.
         * @note component_handle_base needs to be force_cast to component_handle<T> in order to be usable.
         */
        component_handle_base add_component(id_type componentTypeId);

        /**@brief Add component to the entity.
         * @param componentTypeId Type id of component to add.
         * @param value Pointer to component_type that has the starting value you require.
         * @throws legion_invalid_entity_error Thrown when handle's registry reference is invalid.
         * @throws legion_entity_not_found_error Thrown when handle's id is invalid.
         * @throws legion_unknown_component_error Thrown when the component type is unknown.
         * @returns component_handle_base Valid component handle base for the newly created component.
         * @note component_handle_base needs to be force_cast to component_handle<T> in order to be usable.
         */
        component_handle_base add_component(id_type componentTypeId, void* value);

        /**@brief Add component to the entity.
         * @tparam component_type Type of component to add.
         * @throws legion_invalid_entity_error Thrown when handle's registry reference is invalid.
         * @throws legion_entity_not_found_error Thrown when handle's id is invalid.
         * @throws legion_unknown_component_error Thrown when the component type is unknown.
         * @returns component_handle<component_type> Valid component handle for the newly created component.
         */
        template<typename component_type>
        component_handle<component_type> add_component()
        {
            return force_value_cast<component_handle<component_type>>(add_component(typeHash<component_type>()));
        }
        /**@brief Add component to the entity.
         * @param value Starting value of the component.
         * @tparam component_type Type of component to add.
         * @throws legion_invalid_entity_error Thrown when handle's registry reference is invalid.
         * @throws legion_entity_not_found_error Thrown when handle's id is invalid.
         * @throws legion_unknown_component_error Thrown when the component type is unknown.
         * @returns component_handle<component_type> Valid component handle for the newly created component.
         */
        template<typename component_type>
        component_handle<std::remove_reference_t<component_type>> add_component(component_type&& value)
        {
            return force_value_cast<component_handle<std::remove_reference_t<component_type>>>(add_component(typeHash<std::remove_reference_t<component_type>>(), reinterpret_cast<void*>(&value)));
        }

        /**@brief Add component to the entity.
         * @param value Starting value of the component.
         * @tparam component_type Type of component to add.
         * @throws legion_invalid_entity_error Thrown when handle's registry reference is invalid.
         * @throws legion_entity_not_found_error Thrown when handle's id is invalid.
         * @throws legion_unknown_component_error Thrown when the component type is unknown.
         * @returns component_handle<component_type> Valid component handle for the newly created component.
         */
        template<typename component_type>
        component_handle<std::remove_reference_t<component_type>> add_component(component_type& value)
        {
            return add_component(typeHash<std::remove_reference_t<component_type>>(), reinterpret_cast<void*>(&value)).template cast<std::remove_reference_t<component_type>>();
        }

        /**@brief Add multiple components to the entity.
         * @tparam component_type First type of component to add.
         * @tparam component_types The rest of the types of components to add.
         * @throws legion_invalid_entity_error Thrown when handle's registry reference is invalid.
         * @throws legion_entity_not_found_error Thrown when handle's id is invalid.
         * @throws legion_unknown_component_error Thrown when the component type is unknown.
         * @returns Tuple with all the handles.
         */
        template<typename component_type, typename... component_types, typename = doesnt_inherit_from<component_type, archetype_base>>
        auto add_components();

        /**@brief Add multiple components to the entity through the use of an archetype.
         * @tparam archetype_type The type of archetype to add the components of.
         * @throws legion_invalid_entity_error Thrown when handle's registry reference is invalid.
         * @throws legion_entity_not_found_error Thrown when handle's id is invalid.
         * @throws legion_unknown_component_error Thrown when the component type is unknown.
         * @returns Tuple with all the handles.
         */
        template<typename archetype_type, typename = inherits_from<archetype_type, archetype_base>>
        auto add_components();

        /**@brief Add multiple components to the entity.
         * @tparam component_type First type of component to add.
         * @tparam component_types The rest of the types of components to add.
         * @param value Starting value of the first component.
         * @param values... Starting values of the other components.
         * @throws legion_invalid_entity_error Thrown when handle's registry reference is invalid.
         * @throws legion_entity_not_found_error Thrown when handle's id is invalid.
         * @throws legion_unknown_component_error Thrown when the component type is unknown.
         * @returns Tuple with all the handles.
         */
        template<typename component_type, typename... component_types, typename = doesnt_inherit_from<component_type, archetype_base>>
        auto add_components(component_type&& value, component_types&&... values);

        /**@brief Add multiple components to the entity.
         * @tparam component_type First type of component to add.
         * @tparam component_types The rest of the types of components to add.
         * @param value Starting value of the first component.
         * @param values... Starting values of the other components.
         * @throws legion_invalid_entity_error Thrown when handle's registry reference is invalid.
         * @throws legion_entity_not_found_error Thrown when handle's id is invalid.
         * @throws legion_unknown_component_error Thrown when the component type is unknown.
         * @returns Tuple with all the handles.
         */
        template<typename component_type, typename... component_types, typename = doesnt_inherit_from<component_type, archetype_base>>
        auto add_components(component_type& value, component_types&... values);

        /**@brief Add multiple components to the entity through the use of an archetype.
         * @tparam archetype_type The type of archetype to add the components of.
         * @param values... Starting values of the components.
         * @throws legion_invalid_entity_error Thrown when handle's registry reference is invalid.
         * @throws legion_entity_not_found_error Thrown when handle's id is invalid.
         * @throws legion_unknown_component_error Thrown when the component type is unknown.
         * @returns Tuple with all the handles.
         */
        template<typename archetype_type, typename... component_types, typename = inherits_from<archetype_type, archetype_base>>
        auto add_components(component_types&&... values);

        /**@brief Remove component from entity.
         * @param componentTypeId Type id of component to remove.
         * @throws legion_invalid_entity_error Thrown when handle's registry reference is invalid.
         * @throws legion_entity_not_found_error Thrown when handle's id is invalid.
         * @throws legion_unknown_component_error Thrown when the component type is unknown.
         * @note Nothing will happen if the entity doesn't have this component.
         */
        void remove_component(id_type componentTypeId);

        /**@brief Remove component from entity.
         * @tparam component_type Type of component to remove.
         * @throws legion_invalid_entity_error Thrown when handle's registry reference is invalid.
         * @throws legion_entity_not_found_error Thrown when handle's id is invalid.
         * @throws legion_unknown_component_error Thrown when the component type is unknown.
         * @note Nothing will happen if the entity doesn't have this component.
         */
        template<typename component_type>
        void remove_component()
        {
            remove_component(typeHash<component_type>());
        }

        /**@brief Remove multiple components from entity.
         * @tparam component_type Type of the first component to remove.
         * @tparam component_types Types of the other components to remove.
         * @throws legion_invalid_entity_error Thrown when handle's registry reference is invalid.
         * @throws legion_entity_not_found_error Thrown when handle's id is invalid.
         * @throws legion_unknown_component_error Thrown when the component type is unknown.
         * @note Nothing will happen if the entity doesn't have a component.
         */
        template<typename component_type, typename... component_types, typename = doesnt_inherit_from<component_type, archetype_base>>
        void remove_components();

        /**@brief Remove multiple components from entity through an archetype.
         * @tparam archetype_type Type of the archetype with the components to remove.
         * @throws legion_invalid_entity_error Thrown when handle's registry reference is invalid.
         * @throws legion_entity_not_found_error Thrown when handle's id is invalid.
         * @throws legion_unknown_component_error Thrown when the component type is unknown.
         * @note Nothing will happen if the entity doesn't have a component.
         */
        template<typename archetype_type, typename = inherits_from<archetype_type, archetype_base>>
        void remove_components();


        /**@brief Destroy this entity. Destroys entity and invalidates handle. (also destroys all of it's components)
         * @param recurse Destroy all children and children of children as well? Default value is true.
         */
        void destroy(bool recurse = true);

        /**@brief Check whether this entity handle is valid or not.
         * @returns bool True if the handle is pointing to valid entity and the registry reference is also valid.
         */
        bool valid() const;
    };
}

#if !defined(DOXY_EXCLUDE)
namespace std
{
    template<> struct hash<legion::core::ecs::entity_handle>
    {
        std::size_t operator()(legion::core::ecs::entity_handle const& handle) const noexcept
        {
            return std::hash<legion::core::id_type>{}(handle.get_id());
        }
    };
}
#endif

namespace legion::core::ecs
{
    using entity_set = hashed_sparse_set<entity_handle>;
}
