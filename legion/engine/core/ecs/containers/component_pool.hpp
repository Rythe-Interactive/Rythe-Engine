#pragma once
#include <core/platform/platform.hpp>
#include <core/types/primitives.hpp>
#include <core/containers/sparse_map.hpp>

#include <core/ecs/prototypes/component_prototype.hpp>
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
        /**@brief Creates a component attached to a certain entity.
         * @param target Entity ID to create the component for.
         * @return Pointer to the created component.
         */
        virtual void* create_component(id_type target) LEGION_PURE;

        /**@brief Creates a component attached to a certain entity. (uses a prototype to serialize the component)
         * @param target Entity ID to create the component for.
         * @param prototype
         * @return Pointer to the created component.
         */
        virtual void* create_component(id_type target, const serialization::component_prototype_base& prototype) LEGION_PURE;
        virtual void* create_component(id_type target, serialization::component_prototype_base&& prototype) LEGION_PURE;

        /**@brief Check if a certain entity has the type of component managed by this pool.
         * @param target Entity ID of the entity to check for.
         * @return True if the specified entity is registered with this pool, false if not.
         */
        L_NODISCARD virtual bool contains(id_type target) const LEGION_PURE;

        /**@brief
         * @param target
         * @return
         */
        L_NODISCARD virtual void* get_component(id_type target) const LEGION_PURE;

        /**@brief
         * @param target
         * @return
         */
        virtual void destroy_component(id_type target) LEGION_PURE;

        /**@brief
         * @param target
         * @return
         */
        L_NODISCARD virtual std::unique_ptr<serialization::component_prototype_base> create_prototype(id_type target) const LEGION_PURE;
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

        virtual void* create_component(id_type target);
        virtual void* create_component(id_type target, const serialization::component_prototype_base& prototype);
        virtual void* create_component(id_type target, serialization::component_prototype_base&& prototype);

        L_NODISCARD virtual bool contains(id_type target) const;

        L_NODISCARD virtual void* get_component(id_type target) const;

        virtual void destroy_component(id_type target);

        L_NODISCARD virtual std::unique_ptr<serialization::component_prototype_base> create_prototype(id_type target) const;

        static component_type& create_component_direct(id_type target);
        static component_type& create_component_direct(id_type target, const serialization::component_prototype_base& prototype);
        static component_type& create_component_direct(id_type target, serialization::component_prototype_base&& prototype);

        L_NODISCARD static bool contains_direct(id_type target);

        L_NODISCARD static component_type& get_component_direct(id_type target);

        static void destroy_component_direct(id_type target);
    };
}
