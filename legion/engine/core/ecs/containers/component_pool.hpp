#pragma once
#include <core/platform/platform.hpp>
#include <core/types/primitives.hpp>
#include <core/containers/sparse_map.hpp>

#include <core/ecs/prototypes/component_prototype.hpp>
#include <core/ecs/filters/filterregistry.hpp>

namespace legion::core::ecs
{
    struct component_pool_base
    {
        virtual void* create_component(id_type target) LEGION_PURE;
        virtual void* create_component(id_type target, const serialization::component_prototype_base& prototype) LEGION_PURE;

        L_NODISCARD virtual void* get_component(id_type target) const LEGION_PURE;

        virtual void destroy_component(id_type target) LEGION_PURE;
    };

    template<typename component_type>
    struct component_pool : public component_pool_base
    {
        static sparse_map<id_type, component_type> m_components;

        virtual void* create_component(id_type target);
        virtual void* create_component(id_type target, const serialization::component_prototype_base& prototype);
        
        virtual void destroy_component(id_type target);

        L_NODISCARD virtual void* get_component(id_type target) const;


        static component_type& create_component_direct(id_type target);
        static component_type& create_component_direct(id_type target, const serialization::component_prototype_base& prototype);

        static void destroy_component_direct(id_type target);

        L_NODISCARD static component_type& get_component_direct(id_type target);
    };
}
