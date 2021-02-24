#pragma once
#include <core/platform/platform.hpp>
#include <core/types/primitives.hpp>
#include <core/containers/sparse_map.hpp>

#include <core/ecs/prototypes/component_prototype.hpp>

namespace legion::core::ecs
{
    struct component_pool_base
    {
        virtual void* create_component(id_type target) LEGION_PURE;
        virtual void* create_component(id_type target, const serialization::component_prototype_base& prototype) LEGION_PURE;

        virtual void* get_component(id_type target) LEGION_PURE;

        virtual void destroy_component(id_type target) LEGION_PURE;
    };

    template<typename component_type>
    struct component_pool : public component_pool_base
    {
        sparse_map<id_type, component_type> m_components;

        virtual void* create_component(id_type target);
        virtual void* create_component(id_type target, const serialization::component_prototype_base& prototype);
        
        virtual void destroy_component(id_type target);

        virtual void* get_component(id_type target);
    };
}

#include <core/ecs/containers/component_pool.inl>
