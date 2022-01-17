#pragma once
#include <memory>
#include <unordered_set>

#include <core/types/types.hpp>
#include <core/containers/hashed_sparse_set.hpp>

#include <core/ecs/meta/meta.hpp>

/**
 * @file entity.hpp
 */

namespace legion::core
{
    namespace ecs
    {
        struct component_base;

        template<typename component_type>
        struct component;

        struct entity;
    }
}

#if !defined(DOXY_EXCLUDE)
namespace std
{
    template<> struct hash<legion::core::ecs::entity>
    {
        std::size_t operator()(legion::core::ecs::entity const& handle) const noexcept;
    };
}
#endif

namespace legion::core::ecs
{
    using entity_set = hashed_sparse_set<entity>;

    static constexpr id_type world_entity_id = 1;

    struct entity_data;

#pragma region Child Iterators
    struct child_iterator
    {
        friend struct entity;
    private:
        struct impl;

        std::shared_ptr<impl> m_pimpl;
        child_iterator(impl* implptr);

    public:

        friend bool operator==(const child_iterator& lhs, const child_iterator& rhs);

        friend bool operator!=(const child_iterator& lhs, const child_iterator& rhs) { return !(lhs == rhs); }

        entity& operator*();

        entity* operator->();

        child_iterator& operator++();
        child_iterator& operator--();
        child_iterator operator++(int);
        child_iterator operator--(int);

    };

    struct const_child_iterator
    {
        friend struct entity;
    private:
        struct impl;

        std::shared_ptr<impl> m_pimpl;
        const_child_iterator(impl* implptr);

    public:

        friend bool operator==(const const_child_iterator& lhs, const const_child_iterator& rhs);

        friend bool operator!=(const const_child_iterator& lhs, const const_child_iterator& rhs) { return !(lhs == rhs); }

        const entity& operator*();

        const entity* operator->();

        const_child_iterator& operator++();
        const_child_iterator& operator--();
        const_child_iterator operator++(int);
        const_child_iterator operator--(int);

    };
    
    struct child_reverse_iterator
    {
        friend struct entity;
    private:
        struct impl;

        std::shared_ptr<impl> m_pimpl;
        child_reverse_iterator(impl* implptr);

    public:

        friend bool operator==(const child_reverse_iterator& lhs, const child_reverse_iterator& rhs);

        friend bool operator!=(const child_reverse_iterator& lhs, const child_reverse_iterator& rhs) { return !(lhs == rhs); }

        entity& operator*();

        entity* operator->();

        child_reverse_iterator& operator++();
        child_reverse_iterator& operator--();
        child_reverse_iterator operator++(int);
        child_reverse_iterator operator--(int);

    };
    
    struct const_child_reverse_iterator
    {
        friend struct entity;
    private:
        struct impl;

        std::shared_ptr<impl> m_pimpl;
        const_child_reverse_iterator(impl* implptr);

    public:

        friend bool operator==(const const_child_reverse_iterator& lhs, const const_child_reverse_iterator& rhs);

        friend bool operator!=(const const_child_reverse_iterator& lhs, const const_child_reverse_iterator& rhs) { return !(lhs == rhs); }

        const entity& operator*();

        const entity* operator->();

        const_child_reverse_iterator& operator++();
        const_child_reverse_iterator& operator--();
        const_child_reverse_iterator operator++(int);
        const_child_reverse_iterator operator--(int);

    };
#pragma endregion

    /**@struct entity
     * @brief Handle to an entity.
     */
    struct entity
    {
    public:
        /**@brief Pointer to entity specific data.
         */
        entity_data* data;

        /**@brief Equal operator with any numerical types or other entities.
         *        Specializations are for: nullptr_t, id_type, entity.
         */
        template<typename T>
        L_NODISCARD bool operator ==(T val) const;

        /**@brief Not equal operator with any numerical types or other entities.
         *        Specializations are for: nullptr_t, id_type, entity.
         */
        template<typename T>
        L_NODISCARD bool operator !=(T val) const;

        /**@brief Allows entity handles to act like an entity ID.
         * @return ID of the entity.
         */
        L_NODISCARD operator id_type () const noexcept;

        /**@brief Checks if the entity is alive and valid.
         * @return True if the entity is alive and valid, otherwise false.
         * @note There might be more convenient alternatives like: `if(entity)` or `if(entity != nullptr)`
         */
        L_NODISCARD bool valid() const noexcept;

        /**@brief Directly access the entity specific data.
         * @return Returns entity.data
         */
        L_NODISCARD entity_data* operator->() noexcept;
        L_NODISCARD const entity_data* operator->() const noexcept;

        L_NODISCARD std::unordered_set<id_type>& component_composition();
        L_NODISCARD const std::unordered_set<id_type>& component_composition() const;

        /**@brief Replaces current parent with a new one.
         */
        void set_parent(id_type parent);
        void set_parent(entity parent);

        /**@brief Fetches the current parent.
         * @note You could also use entity->parent or entity.data->parent. This will avoid a possible function call.
         *       This function exists for legacy reasons and for API completeness.
         */
        L_NODISCARD entity get_parent() const;

        /**@brief Adds a new child to this entity and thus makes this entity replace the child's current parent.
         */
        void add_child(id_type child);
        void add_child(entity child);

        /**@brief Removes a child from this entity, the world will be the new parent of the child.
         * @note This does NOT destroy the child!
         */
        void remove_child(id_type child);
        void remove_child(entity child);

        /**@brief Removes all children from this entity, the world will be the new parent of the children.
         * @note This does NOT destroy the children!
         */
        void remove_children();

        /**@brief Destroys all children from this entity. This destroys all of the children's components and may destroy their children recursively.
         * @param recurse Whether deeper layers of hierarchy should be destroyed as well or just the upper layers.
         * @note Orphaned children from any child that may have been destroyed will receive the world as their new parent.
         */
        void destroy_children(bool recurse = true);

        /**@brief Fetches set of all child entities of this entity.
         */
        L_NODISCARD entity_set& children();
        L_NODISCARD const entity_set& children() const;

        /**@brief Fetches a specific child in the children list of this entity.
         * @note Children in the list are in no specific ordering due to memory pooling.
         */
        L_NODISCARD entity get_child(size_type index) const;

        /**@brief Gets iterator to the first child.
         */
        L_NODISCARD child_iterator begin();
        L_NODISCARD const_child_iterator begin() const;
        L_NODISCARD const_child_iterator cbegin() const;

        /**@brief Gets reverse iterator to the last child.
         */
        L_NODISCARD child_reverse_iterator rbegin();
        L_NODISCARD const_child_reverse_iterator rbegin() const;
        L_NODISCARD const_child_reverse_iterator crbegin() const;

        /**@brief Gets iterator to the last child.
         */
        L_NODISCARD child_iterator end();
        L_NODISCARD const_child_iterator end() const;
        L_NODISCARD const_child_iterator cend() const;

        /**@brief Gets reverse iterator to the first child.
         */
        L_NODISCARD child_reverse_iterator rend();
        L_NODISCARD const_child_reverse_iterator rend() const;
        L_NODISCARD const_child_reverse_iterator crend() const;

        /**@brief Destroys the entity and it's components. May Destroy all children recursively.
         * @param recurse Whether deeper layers of hierarchy should be destroyed as well or just this entity.
         * @note Orphaned children that aren't destroyed will receive the world as their new parent.
         */
        void destroy(bool recurse = true);

        /**@brief Creates and adds a new component of a certain type to this entity.
         * @tparam component_type Type of the component to add.
         * @return Component handle to the component.
         */
        template<typename component_type>
        wrap_component_t<component_type> add_component();

        template<typename component_type0, typename component_type1, typename... component_typeN>
        component_tuple<component_type0, component_type1, component_typeN...> add_component();

        /**@brief Adds a precreated component of a certain type to this entity.
         * @tparam component_type Type of the component to add.
         * @param value Component value to use.
         * @return Component handle to the component.
         */
        template<typename component_type>
        wrap_component_t<component_type> add_component(component_type&& value);
        template<typename component_type>
        wrap_component_t<component_type> add_component(const component_type& value);
                
        template<typename component_type0, typename component_type1, typename... component_typeN>
        component_tuple<component_type0, component_type1, component_typeN...> add_component(component_type0&& value0, component_type1&& value1, component_typeN&&... valueN);
        template<typename component_type0, typename component_type1, typename... component_typeN>
        component_tuple<component_type0, component_type1, component_typeN...> add_component(const component_type0& value0, const component_type1& value1, const component_typeN&... valueN);

        template<typename archetype_type, typename component_type0, typename component_type1, typename... component_typeN>
        typename archetype_type::handle_group add_component(component_type0&& value0, component_type1&& value1, component_typeN&&... valueN);

        /**@brief Checks whether this entity has a certain component.
         * @tparam component_type Type of the component to check for.
         * @return True if the entity has the component, false if not.
         */
        template<typename component_type>
        L_NODISCARD bool has_component() const;
        template<typename component_type0, typename component_type1, typename... component_typeN>
        L_NODISCARD bool has_component() const;

        /**@brief Gets a component handle to a certain component on this entity. 
         * @tparam component_type Type of the component to get.
         * @return Component handle to the component.
         */
        template<typename component_type>
        L_NODISCARD wrap_component_t<component_type> get_component();
        template<typename component_type>
        L_NODISCARD const_wrap_component_t<component_type> get_component() const;

        template<typename component_type0, typename component_type1, typename... component_typeN>
        L_NODISCARD component_tuple<component_type0, component_type1, component_typeN...> get_component();
        template<typename component_type0, typename component_type1, typename... component_typeN>
        L_NODISCARD const_component_tuple<component_type0, component_type1, component_typeN...> get_component() const;

        /**@brief Removes and destroys a component from this entity.
         * @tparam component_type Type of the component to remove.
         */
        template<typename component_type>
        void remove_component();
        template<typename component_type0, typename component_type1, typename... component_typeN>
        void remove_component();
    };
}
