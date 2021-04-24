#pragma once
#include <core/platform/platform.hpp>
#include <core/types/primitives.hpp>
#include <core/containers/sparse_map.hpp>
#include <core/events/events.hpp>

#include <core/ecs/prototypes/component_prototype.hpp>
#include <core/ecs/containers/component_container.hpp>
#include <core/ecs/handles/entity.hpp>
#include <core/ecs/filters/filterregistry.hpp>

/**
 * @file component_pool.hpp
 */

namespace legion::core::ecs
{
    /**@struct component_pool_base
     * @brief Common base class of all component pools that allows the use of component pools without knowing a component type at compile-time.
     */
    struct component_pool_base
    {
        virtual void clear() LEGION_PURE;

        /**@brief Creates a component attached to a certain entity.
         * @param target Entity ID to create the component for.
         * @return Pointer to the created component.
         */
        virtual void* create_component(entity target) LEGION_PURE;

        /**@brief Creates a component attached to a certain entity. (uses a prototype to serialize the component)
         * @param target Entity ID to create the component for.
         * @param prototype Prototype used to serialize the component.
         * @return Pointer to the created component.
         */
        virtual void* create_component(entity target, const serialization::component_prototype_base& prototype) LEGION_PURE;
        virtual void* create_component(entity target, serialization::component_prototype_base&& prototype) LEGION_PURE;

        /**@brief Check if a certain entity has the type of component managed by this pool.
         * @param target Entity ID of the entity to check for.
         * @return True if the specified entity is registered with this pool, false if not.
         */
        L_NODISCARD virtual bool contains(entity target) const LEGION_PURE;

        /**@brief Fetch the component attached to a certain entity.
         * @param target Entity ID of the entity the component is attached to.
         * @return Pointer to the component.
         */
        L_NODISCARD virtual void* get_component(entity target) const LEGION_PURE;

        /**@brief Erase a component attached to a certain entity.
         * @param target Entity ID of the entity the component is attached to.
         */
        virtual void destroy_component(entity target) LEGION_PURE;

        /**@brief Serialize a certain component into a prototype.
         * @param target Entity ID of the entity the component is attached to.
         * @return `std::unique_ptr` with the prototype.
         */
        L_NODISCARD virtual std::unique_ptr<serialization::component_prototype_base> create_prototype(entity target) const LEGION_PURE;

        virtual ~component_pool_base() = default;
    };

    /**@struct component_pool
     * @brief Storage container for all components of a certain type.
     * @tparam component_type Type of components to handle and store.
     */
    template<typename component_type>
    struct component_pool : public component_pool_base
    {
    private:
        static void* this_ptr;
    public:
        static sparse_map<id_type, component_type> m_components;

        static void reserve(size_type size);

        virtual void clear();

        /**@brief Creates a component attached to a certain entity.
         * @param target Entity ID to create the component for.
         * @return Pointer to the created component.
         */
        virtual void* create_component(entity target);

        /**@brief Creates a component attached to a certain entity. (uses a prototype to serialize the component)
         * @param target Entity ID to create the component for.
         * @param prototype Prototype used to serialize the component.
         * @return Pointer to the created component.
         */
        virtual void* create_component(entity target, const serialization::component_prototype_base& prototype);
        virtual void* create_component(entity target, serialization::component_prototype_base&& prototype);

        /**@brief Check if a certain entity has the type of component managed by this pool.
         * @param target Entity ID of the entity to check for.
         * @return True if the specified entity is registered with this pool, false if not.
         */
        L_NODISCARD virtual bool contains(entity target) const;

        /**@brief Fetch the component attached to a certain entity.
         * @param target Entity ID of the entity the component is attached to.
         * @return Pointer to the component.
         */
        L_NODISCARD virtual void* get_component(entity target) const;

        /**@brief Erase a component attached to a certain entity.
         * @param target Entity ID of the entity the component is attached to.
         */
        virtual void destroy_component(entity target);

        /**@brief Serialize a certain component into a prototype.
         * @param target Entity ID of the entity the component is attached to.
         * @return `std::unique_ptr` with the prototype.
         */
        L_NODISCARD virtual std::unique_ptr<serialization::component_prototype_base> create_prototype(entity target) const;

        L_NODISCARD static serialization::component_prototype<component_type> create_prototype_direct(entity target);

        /**@brief Creates a component attached to a certain entity.
         * @note This is a more optimized direct variant, but it requires compile time info about what the component type is.
         * @param target Entity ID to create the component for.
         * @return Reference to the component.
         */
        static component_type& create_component_direct(entity target);

        /**@brief Attaches a precreated component to a certain entity.
         * @param target Entity ID to create the component for.
         * @param value Component value to use.
         * @return Reference to the component.
         */
        static component_type& create_component_direct(entity target, component_type&& value);
        static component_type& create_component_direct(entity target, const component_type& value);

        /**@brief Creates a component attached to a certain entity. (uses a prototype to serialize the component)
         * @note This is a more optimized direct variant, but it requires compile time info about what the component type is.
         * @param target Entity ID to create the component for.
         * @param prototype Prototype used to serialize the component.
         * @return Reference to the component.
         */
        static component_type& create_component_direct(entity target, const serialization::component_prototype_base& prototype);
        static component_type& create_component_direct(entity target, serialization::component_prototype_base&& prototype);

        /**@brief Check if a certain entity has the type of component managed by this pool.
         * @note This is a more optimized direct variant, but it requires compile time info about what the component type is.
         * @param target Entity ID of the entity to check for.
         * @return True if the specified entity is registered with this pool, false if not.
         */
        L_NODISCARD static bool contains_direct(entity target);

        /**@brief Fetch the component attached to a certain entity.
         * @note This is a more optimized direct variant, but it requires compile time info about what the component type is.
         * @param target Entity ID of the entity the component is attached to.
         * @return Reference to the component.
         */
        L_NODISCARD static component_type& get_component_direct(entity target);

        /**@brief Erase a component attached to a certain entity.
         * @note This is a more optimized direct variant, but it requires compile time info about what the component type is.
         * @param target Entity ID of the entity the component is attached to.
         */
        static void destroy_component_direct(entity target);

        static void fill_container(component_container<component_type>& container, entity_set& entities);
    };
}
